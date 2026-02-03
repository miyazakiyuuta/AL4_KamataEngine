#pragma once
#include "KamataEngine.h"
#include "BaseScene.h"
#include "Player.h"
#include "MapChipField.h"
#include "Collision.h"
#include "Skydome.h"
#include "Fade.h"
#include "CameraController.h"

#include <vector>

class Enemy;

class GamePlayScene : public BaseScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	~GamePlayScene() override;

	void GenerateBlocks(); // ブロック生成

private:
	enum class Phase {
		kFadeIn,  // フェードイン
		kPlay,    // ゲームプレイ
		kDeath,   // デス演出
		kClear,   // クリア演出
		kFadeOut, // フェードアウト
	};

	/* カメラ */
	KamataEngine::Camera camera_;

	CameraController* cameraController_ = nullptr;

	uint32_t whiteTextureHandle_ = 0;
	uint32_t heartTextureHandle_ = 0;

	/* プレイヤー */
	KamataEngine::Model* playerModel_ = nullptr;
	Player* player_ = nullptr;
	KamataEngine::Model* playerBulletModel_ = nullptr;
	KamataEngine::Sprite* playerLifeSprite_[5];
	const float playerLifeSpriteSize = 100.0f;
	float playerInvincibleTimer_ = 0.0f;

	/* ブロック */
	KamataEngine::Model* blockModel_ = nullptr;
	// 二次元配列(vector)
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// 敵
	KamataEngine::Model* enemyModel_ = nullptr;
	Enemy* enemy_ = nullptr;
	bool isBattle_ = false;

	KamataEngine::Sprite* enemyHpSprite_[3];
	const float enemyHpSpriteWidth = 300.0f;
	const float enemyHpSpriteHeight = 30.0f;

	// ゴール
	KamataEngine::Model* goalModel_ = nullptr;
	KamataEngine::WorldTransform goalWorldTransform_;

	Skydome* skydome_ = nullptr;

	// ゲームの現在フェーズ
	Phase phase_;
	Phase prePhase_;

	Fade* fade_ = nullptr;

	bool isFirstUpdate_;

	float cameraEasingTimer_ = 0.0f;
};