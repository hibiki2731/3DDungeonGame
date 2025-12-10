#include "FontTextComponent.h"
#include "Actor.h"
#include "Game.h"

CharFont::CharFont()
{
	mFontSize = 64;
	mFontName = L"MS UI Gothic";
}

CharFont::~CharFont()
{
	ConstBuf3->Unmap(0, nullptr);
}

void CharFont::init(std::shared_ptr<Graphic>& graphic, wchar_t* c, UINT fontSize)
{
	mGraphic = graphic;
	mFontSize = fontSize;

	//フォントハンドルの設定
	UINT fontWeight = 1000;
	LOGFONT lf = {
		mFontSize, 0, 0, 0,
		fontWeight, 0, 0, 0,
		SHIFTJIS_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		DEFAULT_PITCH | FF_MODERN,
		(WCHAR)"Arial",
	};
	//フォントハンドルを生成
	HFONT hFont = CreateFontIndirectW(&lf);
	

	//フォントビットマップ取得のための準備
	HDC   hdc = GetDC(NULL); //現在のウィンドウに適用
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

	;
	//
	UINT  code = (UINT)*c; //出力する文字(一文字)
	const UINT gradFlag = GGO_GRAY4_BITMAP; //17諧調のグレーのグリフビットマップ
	CONST MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };  //何も変換しない

	//pvBufferのメモリサイズを取得
	DWORD size = GetGlyphOutlineW(
		hdc,
		code,
		gradFlag,
		&mGm,
		0,
		NULL,
		&mat
	);
	BYTE* FontBitmap = new BYTE[size];

	//フォントビットマップを取得
	GetGlyphOutlineW(
		hdc,
		code,
		gradFlag,
		&mGm,
		size,
		FontBitmap,
		&mat
	);
	//フォントを貼り付けるいたポリゴンの作成
	createPolygon();

	//シェーダーリソースの作成
	createTexture(mGm, FontBitmap);

	//コンスタントバッファをつくる
	mGraphic->createBuf(mGraphic->alignedSize(sizeof(Cb3)), ConstBuf3);
	mGraphic->mapBuf((void**)&Cb3, ConstBuf3);
	Cb3->world = XMMatrixIdentity();
	Cb3->rect = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	delete[] FontBitmap;
}

ComPtr<ID3D12Resource>& CharFont::getFontTexture()
{
	return mFontTexture;
}

ComPtr<ID3D12Resource>& CharFont::getConstBuf3()
{
	return ConstBuf3;
}

D3D12_VERTEX_BUFFER_VIEW& CharFont::getVertexBufView()
{
	return VertexBufView;
}

D3D12_INDEX_BUFFER_VIEW& CharFont::getIndexBufView()
{
	return IndexBufView;
}

GLYPHMETRICS& CharFont::getmGm()
{
	return mGm;
}

void CharFont::setOrigin(float x, float y)
{
	Cb3->world = XMMatrixTranslation(x, y, 0.0f);
}

void CharFont::createPolygon()
{
	float width = mGm.gmBlackBoxX / mGraphic->getClientWidth();
	float height = mGm.gmBlackBoxY / mGraphic->getClientHeight();
	float charPolyVertices[] = {
		0.0f,  height, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		 width,  height, 1.0f, 0.0f,
		 width, 0.0f, 1.0f, 1.0f
	};

	UINT16 charPolyIndices[] = {
		0, 1, 2,
		2, 1, 3
	};

	{
		//頂点バッファの作成
		UINT sizeInByte = sizeof(float) * 16;
		HRESULT Hr = mGraphic->createBuf(sizeInByte, VertexBuf);
		assert(SUCCEEDED(Hr));

		//頂点バッファに生データをコピー
		Hr = mGraphic->updateBuf(charPolyVertices, sizeInByte, VertexBuf);
		assert(SUCCEEDED(Hr));

		//頂点バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
		VertexBufView.BufferLocation = VertexBuf->GetGPUVirtualAddress();
		VertexBufView.SizeInBytes = sizeInByte;//全バイト数
		VertexBufView.StrideInBytes = sizeof(float) * 4;//１頂点のバイト数
	}
	{
		//インデックスバッファの作成
		UINT sizeInByte = sizeof(UINT16) * 6;
		HRESULT Hr = mGraphic->createBuf(sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファ人生データをコピー
		Hr = mGraphic->updateBuf(charPolyIndices, sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファービューを作る
		IndexBufView.BufferLocation = IndexBuf->GetGPUVirtualAddress();
		IndexBufView.SizeInBytes = sizeInByte;//全バイト数
		IndexBufView.Format = DXGI_FORMAT_R16_UINT;//UINT16
	}
}

void CharFont::createTexture(GLYPHMETRICS& gm, BYTE*& FontBitmap)
{
	int width = (gm.gmBlackBoxX + 3) / 4 * 4;
	int height = gm.gmBlackBoxY;
	int bytePerPixel = 1; //グレースケール
	//１行のピッチを256の倍数にしておく(バッファサイズは256の倍数でなければいけない)
	const UINT64 alignedRowPitch = (width * bytePerPixel + 0xff) & ~0xff;

	//アップロード用中間バッファをつくり、生データをコピーしておく
	ComPtr<ID3D12Resource> uploadBuf;
	{
		//テクスチャではなくフツーのバッファとしてつくる
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = alignedRowPitch * height;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc = { 1,0 };//通常テクスチャなのでアンチェリしない
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		HRESULT hr = mGraphic->getDevice()->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuf));
		assert(SUCCEEDED(hr));

		//生データをuploadbuffに一旦コピー
		uint8_t* mapBuf = nullptr;
		hr = uploadBuf->Map(0, nullptr, (void**)&mapBuf);//マップ
		auto srcAddress = FontBitmap;
		auto originalRowPitch = width * bytePerPixel;
		for (int y = 0; y < height; ++y) {
			memcpy(mapBuf, srcAddress, originalRowPitch);
			//1行ごとの辻褄を合わせる
			srcAddress += originalRowPitch;
			mapBuf += alignedRowPitch;
		}
		uploadBuf->Unmap(0, nullptr);//アンマップ
	}

	//コピー先であるテクスチャバッファを作る
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		HRESULT hr = mGraphic->getDevice()->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(mFontTexture.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));
	}

	//コピー元ロケーションの準備・フットプリント指定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = uploadBuf.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(1);
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignedRowPitch);
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8_UNORM;
	//コピー先ロケーションの準備・サブリソースインデックス指定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = mFontTexture.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	//コマンドリストでコピーを予約
	mGraphic->getCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	//コピー先からテクスチャリソースに切り替える
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFontTexture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	mGraphic->getCommandList()->ResourceBarrier(1, &barrier);
	//uploadBufの内容を破棄する
	mGraphic->getCommandList()->DiscardResource(uploadBuf.Get(), nullptr);
	//コマンドリストを閉じて
	mGraphic->getCommandList()->Close();
	//実行
	ID3D12CommandList* commandLists[] = { mGraphic->getCommandList().Get()};
	mGraphic->getCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	//リソースがGPUに転送されるまで待機する
	mGraphic->waitGPU();

	//コマンドアロケータをリセット
	HRESULT hr = mGraphic->getCommandAllocator()->Reset();
	assert(SUCCEEDED(hr));
	//コマンドリストをリセット
	hr = mGraphic->getCommandList()->Reset(mGraphic->getCommandAllocator().Get(), nullptr);
	assert(SUCCEEDED(hr));

}

void FontTextComponent::initComponent()
{
	mGraphic = mOwner->getGame()->getGraphic();
	mCommandList = mGraphic->getCommandList();

	fontSize = 32.0f;
	isActive = false;
	baseLineX = 0.0f;
	baseLineY = 0.0f;
	maxRow = 16;
}

void FontTextComponent::draw()
{

	//ディスクリプタヒープをＧＰＵにセット
	UINT numDescriptorHeaps = 1;
	mCommandList->SetDescriptorHeaps(numDescriptorHeaps, mCbvTbvHeap.GetAddressOf());
	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hCbvTbvHeap = mCbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	UINT CbvTbvSize = mGraphic->getCbvTbvIncSize();

	for (auto& charFont : mCharFonts)
	{
		//頂点をセット
		mCommandList->IASetVertexBuffers(0, 1, &charFont->getVertexBufView());

		mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap); hCbvTbvHeap.ptr += CbvTbvSize;
		mCommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap); hCbvTbvHeap.ptr += CbvTbvSize;
		//描画。インデックスを使用
		mCommandList->IASetIndexBuffer(&charFont->getIndexBufView());
		mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void FontTextComponent::setText(const std::wstring& text)
{
	HRESULT hr = mGraphic->createCbvTbvHeap(mCbvTbvHeap, 2 * text.size());
	assert(SUCCEEDED(hr));

	auto hCbvTbvHeap = mCbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
	auto CbvTbvSize = mGraphic->getCbvTbvIncSize();
	mCharFonts.clear();

	for (size_t i = 0; i < text.size(); ++i) {
		std::shared_ptr<CharFont> charFont = std::make_shared<CharFont>();
		wchar_t c = text[i];
		charFont->init(mGraphic, &c, fontSize);

		mCharFonts.push_back(charFont);

		mGraphic->createConstantBufferView(charFont->getConstBuf3(), hCbvTbvHeap);			hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->createShaderResourceView(charFont->getFontTexture(), hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
	}

}

void FontTextComponent::setBaseLine(float x, float y)
{
	baseLineX = x;
	baseLineY = y;
}

void FontTextComponent::setFontSize(UINT size)
{
	fontSize = size;
}

bool FontTextComponent::getIsActive()
{
	return isActive;
}

void FontTextComponent::showText()
{
	float originX = baseLineX;
	float originY = baseLineY;
	for (int i = 0; i < mCharFonts.size(); i++) {
		auto charFont = mCharFonts[i];
		GLYPHMETRICS& gm = charFont->getmGm();
		//原点を文字の左下に合わせる
		originX += static_cast<float>(gm.gmptGlyphOrigin.x) / mGraphic->getClientWidth();

		//改行処理
		if (i >= 1 && (i % maxRow) == 0) {
			originX = baseLineX;
			originY -= static_cast<float>(fontSize) / mGraphic->getClientHeight();
		}

		//原点のy座標を調整
		auto nowOriginY = originY + (static_cast<float>(gm.gmptGlyphOrigin.y) - static_cast<float>(gm.gmBlackBoxY)) / mGraphic->getClientHeight();
		//文字の原点をセット
		charFont->setOrigin(originX, nowOriginY);

		//次の文字のx座標を調整
		originX += static_cast<float>(gm.gmBlackBoxX) / mGraphic->getClientWidth();
	}
	isActive = true;
}

void FontTextComponent::closeText()
{
	isActive = false;
}