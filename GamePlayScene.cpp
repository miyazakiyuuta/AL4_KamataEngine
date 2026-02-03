#include "GamePlayScene.h"
#include "Enemy.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "SceneManager.h"
#include "WorldTransformUtility.h"

#include <cassert>

using namespace KamataEngine;
using namespace Collision;

void GamePlayScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	camera_.translation_ = {20, 10, -30};
	camera_.rotation_ = {};
	camera_.UpdateMatrix();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 要素数
	const uint32_t kNumBlockVertical = mapChipField_->GetNumBLockVirtical();     // 行
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBLockHorizontal(); // 列

	 Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(2, kNumBlockVertical - 13);
	//Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(50, kNumBlockVertical - 13); // debug用

	whiteTextureHandle_ = TextureManager::Load("white1x1.png");
	heartTextureHandle_ = TextureManager::Load("heart.png");

	// プレイヤーの初期化
	playerModel_ = Model::CreateFromOBJ("player", true);
	playerBulletModel_ = Model::CreateFromOBJ("player", true);
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPos, playerBulletModel_);
	player_->SetMapChipField(mapChipField_);
	float space = 10.0f;
	for (int i = 0; i < 5; i++) {
		playerLifeSprite_[i] = Sprite::Create(heartTextureHandle_, {space + space * i + playerLifeSpriteSize * i, 720.0f - playerLifeSpriteSize - space});
		playerLifeSprite_[i]->SetSize({playerLifeSpriteSize, playerLifeSpriteSize});
	}

	cameraController_ = new CameraController();
	cameraController_->SetTarget(player_);
	cameraController_->SetCamera(&camera_);
	cameraController_->SetMovableArea({24, 156, 20, 100}); // (left,right,bottom,top)
	cameraController_->SetMovableArea({24, 105, 20, 100});
	cameraController_->Reset();

	// block
	blockModel_ = Model::CreateFromOBJ("block", true);

	// 要素数を変更する
	// 行数を設定
	worldTransformBlocks_.resize(kNumBlockVertical);
	// 列数を設定
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	GenerateBlocks();

	// 敵の初期化
	// enemyModel_ = Model::CreateFromOBJ("cube", true);
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(60, kNumBlockVertical - 11);
	// Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(60, kNumBlockVertical - 16);
	enemyPos.y += 3.0f;
	enemy_ = new Enemy();
	enemy_->Initialize(enemyModel_, &camera_, enemyPos);

	float margin = 5.0f;
	for (int i = 0; i < 3; i++) {
		enemyHpSprite_[i] = Sprite::Create(whiteTextureHandle_, {});
		enemyHpSprite_[i]->SetSize({enemyHpSpriteWidth, enemyHpSpriteHeight});
		enemyHpSprite_[i]->SetPosition({1280.0f - enemyHpSpriteWidth - margin, margin}); // leftTop
	}

	enemyHpSprite_[0]->SetColor({0.0f, 0.0f, 0.0f, 1.0f}); // 黒
	enemyHpSprite_[0]->SetSize({enemyHpSpriteWidth + margin * 2.0f, enemyHpSpriteHeight + margin * 2.0f});
	enemyHpSprite_[0]->SetPosition({1280.0f - enemyHpSpriteWidth - margin * 2.0f, 0.0f});

	enemyHpSprite_[1]->SetColor({1.0f, 0.0f, 0.0f, 1.0f}); // 赤
	enemyHpSprite_[2]->SetColor({0.0f, 1.0f, 0.0f, 1.0f}); // 緑

	goalModel_ = Model::CreateFromOBJ("goal", true);
	goalWorldTransform_.Initialize();
	goalWorldTransform_.translation_ = mapChipField_->GetMapChipPositionByIndex(76, kNumBlockVertical - 12);
	goalWorldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	UpdateWorldTransform(goalWorldTransform_);

	skydome_ = new Skydome();
	skydome_->Initialize();

	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();

	isFirstUpdate_ = true;
}

void GamePlayScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (isFirstUpdate_) {
			isFirstUpdate_ = false;
			fade_->Start(Fade::Status::FadeIn, 1.0f);
		}
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		// ゲームクリア
		AABB goalAABB;
		goalAABB.min = {
		    goalWorldTransform_.translation_.x - goalWorldTransform_.scale_.x / 4.0f,
		    goalWorldTransform_.translation_.y - goalWorldTransform_.scale_.y / 4.0f,
		    goalWorldTransform_.translation_.z - goalWorldTransform_.scale_.z / 4.0f,
		};
		goalAABB.max = {
		    goalWorldTransform_.translation_.x + goalWorldTransform_.scale_.x / 4.0f,
		    goalWorldTransform_.translation_.y + goalWorldTransform_.scale_.y / 4.0f,
		    goalWorldTransform_.translation_.z + goalWorldTransform_.scale_.z / 4.0f,
		};
		if (IsCollision(player_->GetAABB(), goalAABB)) {
			phase_ = Phase::kClear;
		}

		// ゲームオーバー
		if (!player_->GetIsAlive()) {
			phase_ = Phase::kDeath;
		}
		break;
	case Phase::kDeath:
		prePhase_ = phase_;
		fade_->Start(Fade::Status::FadeOut, 2.0f);
		phase_ = Phase::kFadeOut;
		break;
	case Phase::kClear:
		prePhase_ = phase_;
		fade_->Start(Fade::Status::FadeOut, 2.0f);
		phase_ = Phase::kFadeOut;
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			if (prePhase_ == Phase::kDeath) {
				GetSceneManager()->SetNextScene(new GameOverScene());
			} else if (prePhase_ == Phase::kClear) {
				GetSceneManager()->SetNextScene(new GameClearScene());
			}
		}
		break;
	}

	// 行列を更新する
	camera_.UpdateMatrix();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	// プレイヤーの更新
	if (player_->GetIsAlive()) {
		player_->Update();
	}
	if (player_->GetTranslation().y < 0.0f) { // 落ちたら死ぬ
		player_->SetIsAlive(false);
	}

	/*
	// 自機の位置が一定ラインを越えたらカメライージング
	if (player_->GetTranslation().x >= enemy_->GetPosition().x - 30.0f) {
	    cameraEasingTimer_ += 1.0f / 60.0f;
	    float easedT = cameraEasingTimer_;
	    //enemy_->SetIsAttack(true);
	    //float start = player_->GetTranslation().x;
	    float start = enemy_->GetPosition().x - 30.0f;
	    float end = enemy_->GetPosition().x - 20.0f;
	    float cameraAreaX = start * (1.0f - easedT) + end * easedT;
	    cameraAreaX;
	    cameraController_->SetMovableArea({cameraAreaX, 105, 20, 100}); // (left,right,bottom,top)

	    if (player_->GetTranslation().x <= enemy_->GetPosition().x - 30.0f) {
	        Vector3 pos = {player_->GetTranslation().x - 30.0f, player_->GetTranslation().y, player_->GetTranslation().z};
	        player_->SetTranslation(pos);
	    }
	}
	*/

	if (player_->GetTranslation().x >= enemy_->GetPosition().x - 30.0f) {
		if (enemy_->GetIsAlive()) {
			isBattle_ = true;
		}
	}
	if (isBattle_) {
		enemy_->SetIsAttack(true);
		enemy_->SetTargetPosX(player_->GetTranslation().x); // 落下攻撃用の座標設定
		// バトル中は戻れない
		/*if (player_->GetTranslation().x <= mapChipField_->GetMapChipPositionByIndex(60, mapChipField_->GetNumBLockVirtical() - 11).x - 30.0f) {
			Vector3 pos = {mapChipField_->GetMapChipPositionByIndex(60, mapChipField_->GetNumBLockVirtical() - 11).x - 30.0f, player_->GetTranslation().y, player_->GetTranslation().z};
			player_->SetTranslation(pos);
		}*/
		// バトルフィールド外に出ない
		Vector3 rightPos = mapChipField_->GetMapChipPositionByIndex(63, mapChipField_->GetNumBLockVirtical() - 11);
		if (player_->GetTranslation().x >= rightPos.x) {
			Vector3 pos = {rightPos.x, player_->GetTranslation().y, player_->GetTranslation().z};
			player_->SetTranslation(pos);
		}
		// カメラも制御
		if (player_->GetTranslation().x >= enemy_->GetPosition().x - 15.0f) {
			cameraController_->SetMovableArea({enemy_->GetPosition().x - 20.0f, 105, 20, 100});
		}

		if (enemy_->GetHp() <= 0) {
			enemy_->SetIsAttack(false);
			isBattle_ = false;
			cameraController_->SetMovableArea({enemy_->GetPosition().x - 20.0f, 156, 20, 100}); // 156まである(a~z+aa~az+ba~bz)
		}
	}

	cameraController_->Update();

	// 敵の更新
	enemy_->Update();

	enemyHpSprite_[2]->SetSize({enemyHpSpriteWidth * float(enemy_->GetHp()) / float(enemy_->GetMaxHp()), enemyHpSpriteHeight});

	UpdateWorldTransform(goalWorldTransform_);

#pragma region 当たり判定
	playerInvincibleTimer_ -= 1.0f / 60.0f;
	if (playerInvincibleTimer_ <= 0.0f) {
		playerInvincibleTimer_ = 0.0f;
	}

	if (enemy_->GetIsAlive()) {
		// 敵と自機の弾の当たり判定
		AABB enemyAABB = enemy_->GetAABB();
		if (player_->IsBulletCollision(enemyAABB)) {
			enemy_->SetHp(enemy_->GetHp() - 1);
		}
		if (playerInvincibleTimer_ <= 0.0f) {
			// 敵と自機の当たり判定
			if (IsCollision(player_->GetAABB(), enemy_->GetAABB())) {
				player_->SetLife(player_->GetLife() - 1);
				playerInvincibleTimer_ = 3.0f;
			}
		}
	}
	// 敵の弾と自機の当たり判定
	AABB playerAABB = player_->GetAABB();
	if (enemy_->IsBulletCollision(playerAABB)) {
		if (playerInvincibleTimer_ <= 0.0f) {
			player_->SetLife(player_->GetLife() - 1);
			playerInvincibleTimer_ = 3.0f;
		}
	}

	auto pBullets = player_->GetBullets();
	auto eBullets = enemy_->GetBullets();

	for (int i = 0; i < player_->GetMaxBullets(); ++i) {
		PlayerBullet* pb = pBullets[i];
		if (!pb->GetIsAlive()) {
			continue;
		}
		for (int j = 0; j < enemy_->GetMaxBullets(); ++j) {
			EnemyBullet* eb = eBullets[j];
			if (!eb->GetIsAlive()) {
				continue;
			}
			if (IsCollision(pb->GetAABB(), eb->GetAABB())) {
				pb->Kill();
				eb->Kill();
			}
		}
	}
#pragma endregion

	// 敵の弾の照準設定
	enemy_->SetBulletTargetPos(player_->GetTranslation());

	skydome_->Update();
}

void GamePlayScene::Draw() {

	// DirectXCommonインスタンスの取得
	// DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// assert(dxCommon);

	Model::PreDraw();

	if (player_->GetIsAlive() && int(playerInvincibleTimer_ * 60.0f) % 2 == 0) {
		player_->Draw();
	}

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 敵の描画
	enemy_->Draw();

	goalModel_->Draw(goalWorldTransform_, camera_);

	skydome_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw();

	for (int i = 0; i < 5; i++) {
		if (i < player_->GetLife()) {
			playerLifeSprite_[i]->Draw();
		}
	}

	if (isBattle_) {
		for (auto sprite : enemyHpSprite_) {
			sprite->Draw();
		}
	}

	Sprite::PostDraw();

	if (phase_ == Phase::kFadeIn || phase_ == Phase::kFadeOut) {
		fade_->Draw();
	}
}

void GamePlayScene::Finalize() {}

GamePlayScene::~GamePlayScene() {
	delete player_;
	delete enemy_;
}

void GamePlayScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBLockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBLockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kNormalBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j]->scale_ = {2.0f, 2.0f, 2.0f};
			}
		}
	}
}
