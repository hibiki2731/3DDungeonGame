#include "WinMain.h"


INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT) {
	
	std::shared_ptr<Graphic> graphic = std::make_shared<Graphic>();
	graphic->init();
	graphic->clearColor(0.25f, 0.5f, 0.9f);

	//デバック用
	HRESULT hr;

	
		//ビューマトリックス		
		float h = 0.5f;
		XMFLOAT3 eye = { 0, h, -5 }, focus = { 0, 0, 0 }, up{ 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, graphic->getAspect(), 1.0f, 50.0f);
		XMMATRIX viewProj = view * proj;
		graphic->updateViewProj(viewProj);

		//ライトベクトル
		XMFLOAT4 lightPos = { 0, 1, -1, 0 };
		XMVECTOR v = XMLoadFloat4(&lightPos);
		XMVECTOR n = XMVector3Normalize(v);
		XMStoreFloat4(&lightPos, n);
		graphic->updateLightPos(lightPos);

		const char* text[] = {
			"assets\\desk_refined.txt",
		};

		//メッシュの生成
		const int num = _countof(text);
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(graphic);
		meshes.push_back(mesh);

		for (int i = 0; i < num; i++) {
			meshes[i]->create(text[i]);
		}

		//描画用
		int idx = 0;
		float radian = 0;
	

	//メインループ
	while (!graphic->quit()) {
		
		meshes[idx]->ry(sinf(radian));
		meshes[idx]->update();
		//描画
		graphic->beginRender();
		meshes[idx]->draw();
		graphic->endRender();

	}

	//終了処理
	{
		graphic->waitGPU();
		graphic->closeEventHandle();
	}
	//return graphic->msg_wparam();	
	return 0;
}