#include "GameScene.h"
#include "WorldTransformUtility.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	camera_.translation_ = {17, 7, -30};
	camera_.rotation_ = {};
	camera_.UpdateMatrix();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(2, 7);

	// プレイヤーの初期化
	playerModel_ = Model::CreateFromOBJ("cube", true);
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPos);
	player_->SetMapChipField(mapChipField_);

	// block
	blockModel_ = Model::CreateFromOBJ("cube", true);

	// 要素数
	const uint32_t kNumBlockVertical = 10;   // 行
	const uint32_t kNumBlockHorizontal = 20; // 列

	// 要素数を変更する
	// 行数を設定
	worldTransformBlocks_.resize(kNumBlockVertical);
	// 列数を設定
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	GenerateBlocks();

}

void GameScene::Update() {

	// 行列を更新する
	camera_.UpdateMatrix();

	// プレイヤーの更新
	player_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			UpdateWorldTransform(*worldTransformBlock);
		}
	}


}

void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	assert(dxCommon);

	Model::PreDraw();

	player_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();

}

GameScene::~GameScene() {
	delete player_;
}

void GameScene::GenerateBlocks() {
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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}