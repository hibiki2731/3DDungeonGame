#include "AssetManager.h"
#include <string>
#include <fstream>

AssetManager::AssetManager(const std::shared_ptr<Graphic>& graphic)
{
	mGraphic = graphic;
	mCBEndIndex = mGraphic->alignedSize(sizeof(Base3DData));
	mHeapEndIndex = 0;
}

AssetManager::~AssetManager()
{
}

void AssetManager::create(ObjectName objectName)
{
	if (mLoadData.contains(objectName)) return; //すでに読み込まれたオブジェクトはスルー

	std::string fileName;
	
	switch (objectName) {
	case ObjectName::ROCK_WALL_SIDE:
		fileName = "assets\\rock_side\\wall_side.txt";
		break;
	case ObjectName::ROCK_WALL:
		fileName = "assets\\rockObj\\rockWall.txt";
		break;
	case ObjectName::ROCK_FLOOR:
		fileName = "assets\\rockObj\\rockFloor.txt";
		break;
	case ObjectName::SLIME:
		fileName = "assets\\slime.txt";
		break;
	default:
		assert();
	}

	//ファイルを読み込む
	std::ifstream file(fileName);
	assert(!file.fail());

	std::shared_ptr<MeshData> meshData = std::make_shared<MeshData>();

	//メッシュパーツ数を読み込み、メモリを確保
	int numParts = 0;
	file >> numParts;
	meshData->NumParts = numParts;
	meshData->NumVertices.resize(numParts);
	meshData->VertexBuf.resize(numParts);
	meshData->VertexBufView.resize(numParts);
	meshData->Material.resize(3 * numParts); 
	meshData->TextureName.resize(numParts);

	//パーツごとのデータを読み込む
	for (int k = 0; k < numParts; k++) {
		//頂点バッファ
		{
			//生データをファイルからvector配列に読み込む
			//　データチェック
			std::string dataType;
			file >> dataType;
			assert(dataType == "vertices");
			//　頂点数
			int numVertices = 0;
			file >> numVertices;//頂点数
			//　vector配列に読み込む
			UINT NumElementsPerVertex = 8;
			int NumElements = numVertices * NumElementsPerVertex;
			std::vector<float>vertices(NumElements);
			for (int i = 0; i < NumElements; i++) {
				file >> vertices[i];
			}

			//インデックスを使用しない描画の時に、これを使用するので取っておく
			meshData->NumVertices[k] = numVertices;

			//頂点バッファをつくる
			UINT sizeInByte = sizeof(float) * NumElements;//全バイト数
			HRESULT hr = mGraphic->createBuf(sizeInByte,meshData->VertexBuf[k]);
			assert(SUCCEEDED(hr));

			//頂点バッファに生データをコピー
			hr = mGraphic->updateBuf(vertices.data(), sizeInByte, meshData->VertexBuf[k]);
			assert(SUCCEEDED(Hr));

			//位置バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
			meshData->VertexBufView[k].BufferLocation = meshData->VertexBuf[k]->GetGPUVirtualAddress();
			meshData->VertexBufView[k].SizeInBytes = sizeInByte;//全バイト数
			meshData->VertexBufView[k].StrideInBytes = sizeof(float) * NumElementsPerVertex;//１頂点のバイト数
		}
		//マテリアル
		{
			//生データをファイルからvector配列に読み込む
			std::string dataType;
			file >> dataType;
			assert(dataType == "material");
			XMFLOAT4 ambient, diffuse, specular;
			file >> ambient.x >> ambient.y >> ambient.z >> ambient.w;
			file >> diffuse.x >> diffuse.y >> diffuse.z >> diffuse.w;
			file >> specular.x >> specular.y >> specular.z >> specular.w;

			meshData->Material[k * 3] = ambient;
			meshData->Material[k * 3 + 1] = diffuse;
			meshData->Material[k * 3 + 2] = specular;
		}
		//テクスチャバッファ
		{
			//ファイル名を読み込む
			std::string dataType;
			file >> dataType;
			assert(dataType == "texture");
			std::string textureFileName;
			std::getline(file, textureFileName);
			textureFileName.erase(0, 1); //先頭の" "を削除

			meshData->TextureName[k] = textureFileName;

		}
	}

	mLoadData[objectName] = meshData;
}

int AssetManager::getCBEndIndex()
{
	return mCBEndIndex;
}

int AssetManager::getHeapEndIndex()
{
	return mHeapEndIndex;
}

std::shared_ptr<MeshData> AssetManager::getMeshData(ObjectName objectName)
{
	auto iter = mLoadData.find(objectName);
	if (iter != mLoadData.end()) {
		return iter->second;
	}
	else {
		create(objectName);
		return mLoadData[objectName];
	}
}

void AssetManager::proceedCBIndex(int size)
{
	mCBEndIndex += size;
}

void AssetManager::proceedHeapIndex(int size)
{
	mHeapEndIndex += size;
}


