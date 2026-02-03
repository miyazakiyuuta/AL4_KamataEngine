#pragma once
#include "BaseScene.h"
#include "KamataEngine.h"
#include "Skydome.h"
#include "Fade.h"

class SceneManager;

class TitleScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	KamataEngine::Camera camera_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	Skydome* skydome_ = nullptr;

	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	Fade* fade_ = nullptr;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	bool isFirstUpdate_;
};
