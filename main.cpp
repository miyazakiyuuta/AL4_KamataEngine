#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

GameScene* gameScene = nullptr;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2B_25_ミヤザキ_ユウタ_AL4");

	gameScene = new GameScene();
	gameScene->Initialize();


	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();

		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();

	}

	delete gameScene;

	// エンジンの終了
	KamataEngine::Finalize();
	
	return 0;
}
