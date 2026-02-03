#include "GameClearScene.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Matrix4x4.h"

#include <cassert>

using namespace KamataEngine;
using namespace MatrixMath;

void GameClearScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	camera_.translation_ = {0, 0, -5};
	camera_.rotation_ = {};
	camera_.UpdateMatrix();

	model_ = Model::CreateFromOBJ("gameClear", true);

	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.rotation_ = {0.0f, -3.14f, 0.0f};

	skydome_ = new Skydome();
	skydome_->Initialize();

	fade_ = new Fade();
	fade_->Initialize();
	
	isFirstUpdate_ = true;
}

void GameClearScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (isFirstUpdate_) {
			isFirstUpdate_ = false;
			fade_->Start(Fade::Status::FadeIn, 2.0f);
		}
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 2.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			GetSceneManager()->SetNextScene(new TitleScene());
		}
		break;
	}

	// 行列を更新する
	camera_.UpdateMatrix();

	

	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファへの書き込み
	worldTransform_.TransferMatrix();
}

void GameClearScene::Draw() {
	// DirectXCommonインスタンスの取得
	//DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	//assert(dxCommon);

	Model::PreDraw();

	model_->Draw(worldTransform_, camera_);

	skydome_->Draw(camera_);

	Model::PostDraw();
	fade_->Draw();
}

void GameClearScene::Finalize() {}
