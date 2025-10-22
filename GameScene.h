#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "MapChipField.h"

#include <vector>

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();
	~GameScene();

	void GenerateBlocks(); // ブロック生成

private:
	/* カメラ */
	KamataEngine::Camera camera_;

	/* プレイヤー */
	KamataEngine::Model* playerModel_ = nullptr;
	Player* player_ = nullptr;

	/* ブロック */
	KamataEngine::Model* blockModel_ = nullptr;
	// 二次元配列(vector)
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;
};