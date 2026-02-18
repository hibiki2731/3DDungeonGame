#include "MeshComponent.h"

#include<fstream>
#include<vector>
#include "Graphic.h"
#include "Actor.h"
#include "Game.h"



MeshComponent::~MeshComponent()
{
	for (int k = 0; k < NumParts; k++) {
		Parts[k].ConstBuf2->Unmap(0, nullptr);
	}
	delete[] Parts;
	ConstBuf1->Unmap(0, nullptr);
}

void MeshComponent::initComponent() {
	mGraphic = mOwner->getGame()->getGraphic();
	mCommandList = mGraphic->getCommandList();
	mOwner->getGame()->addMesh(dynamic_pointer_cast<MeshComponent>(shared_from_this()));
}

void MeshComponent::endProccess()
{
	//Gameからメッシュを削除
	mOwner->getGame()->removeMesh(dynamic_pointer_cast<MeshComponent>(shared_from_this()));
}

void MeshComponent::create(const char* filename)
{	

	//world matrix用コンスタントバッファ1をつくる
	mGraphic->createBuf(mGraphic->alignedSize(sizeof(Cb1)), ConstBuf1);
	mGraphic->mapBuf((void**)&Cb1, ConstBuf1);
	//このメッシュの全パーツで共有するコピー元ディスクリプタヒープをつくる
	mGraphic->createSharedCbvTbvHeap(Cb1vHeap, 1);
	mGraphic->createConstantBufferView(ConstBuf1, Cb1vHeap->GetCPUDescriptorHandleForHeapStart());

	//メッシュのテキストデータを開く
	std::ifstream file(filename);
	assert(!file.fail());

	//メッシュパーツ数を読み込み、メモリを確保
	file >> NumParts;
	Parts = new PARTS[NumParts];

	//パーツごとに各バッファをつくる
	for (int k = 0; k < NumParts; k++) {
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
			Parts[k].NumVertices = numVertices;

			//頂点バッファをつくる
			UINT sizeInByte = sizeof(float) * NumElements;//全バイト数
			Hr = mGraphic->createBuf(sizeInByte, Parts[k].VertexBuf);
			assert(SUCCEEDED(Hr));

			//頂点バッファに生データをコピー
			Hr = mGraphic->updateBuf(vertices.data(), sizeInByte, Parts[k].VertexBuf);
			assert(SUCCEEDED(Hr));

			//位置バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
			Parts[k].VertexBufView.BufferLocation = Parts[k].VertexBuf->GetGPUVirtualAddress();
			Parts[k].VertexBufView.SizeInBytes = sizeInByte;//全バイト数
			Parts[k].VertexBufView.StrideInBytes = sizeof(float) * NumElementsPerVertex;//１頂点のバイト数
		}
		//コンスタントバッファ２
		{
			//生データをファイルからvector配列に読み込む
			std::string dataType;
			file >> dataType;
			assert(dataType == "material");
			XMFLOAT4 ambient, diffuse, specular;
			file >> ambient.x >> ambient.y >> ambient.z >> ambient.w;
			file >> diffuse.x >> diffuse.y >> diffuse.z >> diffuse.w;
			file >> specular.x >> specular.y >> specular.z >> specular.w;


			//コンスタントバッファ２をつくる
			Hr = mGraphic->createBuf(mGraphic->alignedSize(sizeof(MaterialConstBuf)), Parts[k].ConstBuf2);
			assert(SUCCEEDED(Hr));

			//マップして更新。unmapしない。
			Hr = mGraphic->mapBuf((void**)&Parts[k].Cb2, Parts[k].ConstBuf2);
			assert(SUCCEEDED(Hr));
			Parts[k].Cb2->ambient = ambient;
			Parts[k].Cb2->diffuse = diffuse;
			Parts[k].Cb2->specular = specular;

			Parts[k].Cb2->flashColor = XMFLOAT3(1.0f, 1.0f, 1.0f);	//白く光る
			Parts[k].Cb2->flashIntensity = 0.0f;					//最初は光らない
		}
		//テクスチャバッファ
		{
			//ファイル名を読み込む
			std::string dataType;
			file >> dataType;
			assert(dataType == "texture");
			std::string filename;
			std::getline(file, filename);
			filename.erase(0, 1); //先頭の" "を削除

			//ファイルを読み込み、テクスチャバッファをつくる
			Hr = mGraphic->createShaderResource(filename, Parts[k].TextureBuf);
			assert(SUCCEEDED(Hr));
			
		}
	}

	//NumParts分の「ディスクリプタヒープ」をつくる
	NumDescriptors = 4;//ひとつのパーツで使用するディスクリプタの数
	Hr = mGraphic->createCbvTbvHeap(CbvTbvHeap, NumDescriptors * NumParts);
	assert(SUCCEEDED(Hr));
	//バッファの「ビュー」を「ディスクリプタヒープ」にならべてコピーしたり、つくったりする
	auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
	CbvTbvSize = mGraphic->getCbvTbvIncSize();
	for (int k = 0; k < NumParts; k++) {
		mGraphic->copySharedCbvTbvHeap(mGraphic->getCb0vHeap(), hCbvTbvHeap);	hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->copySharedCbvTbvHeap(Cb1vHeap, hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->createConstantBufferView(Parts[k].ConstBuf2, hCbvTbvHeap);			hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->createShaderResourceView(Parts[k].TextureBuf, hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
	}

}

void MeshComponent::draw()
{
	//ワールドマトリックス
	XMMATRIX world = XMMatrixIdentity()
		* XMMatrixScaling(mOwner->getScale().x, mOwner->getScale().y, mOwner->getScale().z)
		* XMMatrixRotationX(mOwner->getRotation().x)
		* XMMatrixRotationY(mOwner->getRotation().y)
		* XMMatrixRotationZ(mOwner->getRotation().z)
		* XMMatrixTranslation(mOwner->getPosition().x, mOwner->getPosition().y, mOwner->getPosition().z)
		;
	Cb1->world = world;


	//ディスクリプタヒープをＧＰＵにセット
	UINT numDescriptorHeaps = 1;
	mCommandList->SetDescriptorHeaps(numDescriptorHeaps, CbvTbvHeap.GetAddressOf());
	//パーツごとに描画
	for (int k = 0; k < NumParts; ++k)
	{
		//頂点をセット
		mCommandList->IASetVertexBuffers(0, 1, &Parts[k].VertexBufView);

		//ディスクリプタヒープをディスクリプタテーブルにセット
		auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		hCbvTbvHeap.ptr += CbvTbvSize * NumDescriptors * k;
		mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//描画。インデックスを使用しない
		mCommandList->DrawInstanced(Parts[k].NumVertices, 1, 0, 0);
	}
}

void MeshComponent::updateFlashIntensity(float intensity)
{
	for (int k = 0; k < NumParts; k++) {
		Parts[k].Cb2->flashIntensity = intensity;
	}
}
