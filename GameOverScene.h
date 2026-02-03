#pragma once
#include "BaseScene.h"
#include "KamataEngine.h"
#include "Skydome.h"
#include "Fade.h"

class GameOverScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	KamataEngine::Camera camera_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	Skydome* skydome_ = nullptr;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	bool isFirstUpdate_;

	Fade* fade_ = nullptr;
};
