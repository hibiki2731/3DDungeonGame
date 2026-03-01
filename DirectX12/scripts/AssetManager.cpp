#include "AssetManager.h"
#include <string>
#include <fstream>

AssetManager::AssetManager(Graphic* graphic)
{
	mGraphic = graphic;
	mCBEndIndex = mGraphic->alignedSize(sizeof(Base3DData));
	mHeapEndIndex = 0;
}

AssetManager::~AssetManager()
{
}

void AssetManager::create(MeshName objectName)
{
	if (mLoadData.contains(objectName)) return; //すでに読み込まれたオブジェクトはスルー

	//jsonファイルを読み込む
	std::ifstream jsonFile("assets\\data\\meshData.json");
	assert(!jsonFile.fail());

	nlohmann::json meshJson;
	jsonFile >> meshJson;

	MeshFileData meshFileData;
	auto meshName = magic_enum::enum_name(objectName); //MeshNameを文字列に変換
	if (meshJson.contains(meshName)) {
		meshFileData.filePath = meshJson[meshName]["filePath"].get<std::string>();
		meshFileData.scale = meshJson[meshName].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f});
	}
	else {
		assert(0 && "MeshNameがjsonファイルに存在しません");
	}
	
	//ファイルを読み込む
	std::ifstream meshFile(meshFileData.filePath);
	assert(!meshFile.fail());

	auto meshData = std::make_unique<MeshData>();

	//メッシュパーツ数を読み込み、メモリを確保
	int numParts = 0;
	meshFile >> numParts;
	meshData->NumParts = numParts;
	meshData->NumVertices.resize(numParts);
	meshData->VertexBuf.resize(numParts);
	meshData->VertexBufView.resize(numParts);
	meshData->Material.resize(3 * numParts); 
	meshData->TextureBuf.resize(numParts);
	meshData->Scale = XMFLOAT3(meshFileData.scale[0], meshFileData.scale[1], meshFileData.scale[2]);

	//パーツごとのデータを読み込む
	for (int k = 0; k < numParts; k++) {
		//頂点バッファ
		{
			//生データをファイルからvector配列に読み込む
			//　データチェック
			std::string dataType;
			meshFile >> dataType;
			assert(dataType == "vertices");
			//　頂点数
			int numVertices = 0;
			meshFile >> numVertices;//頂点数
			//　vector配列に読み込む
			UINT NumElementsPerVertex = 8;
			int NumElements = numVertices * NumElementsPerVertex;
			std::vector<float>vertices(NumElements);
			for (int i = 0; i < NumElements; i++) {
				meshFile >> vertices[i];
			}

			//インデックスを使用しない描画の時に、これを使用するので取っておく
			meshData->NumVertices[k] = numVertices;

			//頂点バッファをつくる
			UINT sizeInByte = sizeof(float) * NumElements;//全バイト数
			HRESULT hr = mGraphic->createBuf(sizeInByte,meshData->VertexBuf[k]);
			assert(SUCCEEDED(hr));

			//頂点バッファに生データをコピー
			hr = mGraphic->updateBuf(vertices.data(), sizeInByte, meshData->VertexBuf[k]);
			assert(SUCCEEDED(hr));

			//位置バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
			meshData->VertexBufView[k].BufferLocation = meshData->VertexBuf[k]->GetGPUVirtualAddress();
			meshData->VertexBufView[k].SizeInBytes = sizeInByte;//全バイト数
			meshData->VertexBufView[k].StrideInBytes = sizeof(float) * NumElementsPerVertex;//１頂点のバイト数
		}
		//マテリアル
		{
			//生データをファイルからvector配列に読み込む
			std::string dataType;
			meshFile >> dataType;
			assert(dataType == "material");
			XMFLOAT4 ambient, diffuse, specular;
			meshFile >> ambient.x >> ambient.y >> ambient.z >> ambient.w;
			meshFile >> diffuse.x >> diffuse.y >> diffuse.z >> diffuse.w;
			meshFile >> specular.x >> specular.y >> specular.z >> specular.w;

			meshData->Material[k * 3] = ambient;
			meshData->Material[k * 3 + 1] = diffuse;
			meshData->Material[k * 3 + 2] = specular;
		}
		//テクスチャバッファ
		{
			//ファイル名を読み込む
			std::string dataType;
			meshFile >> dataType;
			assert(dataType == "texture");
			std::string textureFileName;
			std::getline(meshFile, textureFileName);
			textureFileName.erase(0, 1); //先頭の" "を削除

			auto iter = mTextureData.find(textureFileName);
			if (iter != mTextureData.end()) {
				//すでに読み込まれたテクスチャはスルー
				meshData->TextureBuf[k] = iter->second;
				continue;
			}

			HRESULT hr = mGraphic->createShaderResource(textureFileName, meshData->TextureBuf[k]);
			assert(SUCCEEDED(hr));
			mTextureData[textureFileName] = meshData->TextureBuf[k];

		}
	}

	mLoadData[objectName] = std::move(meshData);
}

int AssetManager::getCBEndIndex(int size)
{
	int index = 0;
	//解放されたメモリがあれば優先して使う
	for (auto iter = mClearedMemory.begin(); iter != mClearedMemory.end(); iter++) {
		//要求サイズより、空いているサイズが大きければ使用する
		if (iter->size >= size) {
			iter->size -= size;	//要求サイズ分メモリを使用
			index = iter->index;
			iter->index += size;//インデックスもそれに応じて移動

			//空いているメモリがなくなれば、配列から除去
			if (iter->size == 0) {
				std::swap(*iter, mClearedMemory.back());
				mClearedMemory.pop_back();
			}
			return index;
		}
	}
	
	//解放されたメモリがなければ、最後尾のインデックスを取得
	index = mCBEndIndex;
	mCBEndIndex += size;
	return index;
}

int AssetManager::getHeapEndIndex(int size)
{
	int index = 0;
	//解放されたヒープがあれば優先して使う
	for (auto iter = mClearedHeap.begin(); iter != mClearedHeap.end(); iter++) {
		//要求サイズより、空いているサイズが大きければ使用する
		if (iter->size >= size) {
			iter->size -= size; //要求サイズ分ヒープを使用
			index = iter->index;
			iter->index += size;//インデックスもそれに応じて移動

			//空いているヒープがなくなれば、配列から除去
			if (iter->size == 0) {
				std::swap(*iter, mClearedHeap.back());
				mClearedHeap.pop_back();
			}
			return index;
		}
	}

	//解放されたヒープがなければ、最後尾のインデックスを取得
	index = mHeapEndIndex;
	mHeapEndIndex += size;
	return index;
}

MeshData* AssetManager::getMeshData(MeshName objectName)
{
	auto iter = mLoadData.find(objectName);
	if (iter != mLoadData.end()) {
		return iter->second.get();
	}
	else {
		create(objectName);
		return mLoadData[objectName].get();
	}
}

void AssetManager::deleteMemory(int index, int size)
{
	ClearedMemory memory = { index, size };
	mClearedMemory.emplace_back(memory);
}

void AssetManager::deleteHeap(int index, int size)
{
	ClearedHeap heap = { index, size };
	mClearedHeap.emplace_back(heap);
}


