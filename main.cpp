#include <Windows.h>
#include "KamataEngine.h"
#include "Game.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2B_25_ミヤザキ_ユウタ_OrangeGun");

	Game* game = new Game();
	game->Initialize();


	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		game->Update();

		// 描画開始
		dxCommon->PreDraw();

		game->Draw();

		// 描画終了
		dxCommon->PostDraw();

	}

	delete game;

	// エンジンの終了
	KamataEngine::Finalize();
	
	return 0;
}
