#pragma once
#include "KamataEngine.h"

/// <summary>
/// 天球
/// </summary>
class Skydome {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const KamataEngine::Camera& camera);

	~Skydome();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform* worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
};
