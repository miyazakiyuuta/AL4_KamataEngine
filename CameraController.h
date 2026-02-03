#pragma once
#include "KamataEngine.h"

// 矩形
struct Rect {
	float left = 0.0f;   // 左端
	float right = 1.0f;  // 右端
	float bottom = 0.0f; // 下端
	float top = 1.0f;    // 上端
};

class Player;

class CameraController {
public:
	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	void SetMovableArea(const Rect& area) { movableArea_ = area; }

private:

	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3 v1, const KamataEngine::Vector3& v2, float t) {
		KamataEngine::Vector3 result;
		result.x = v1.x * (1.0f - t) + v2.x * t;
		result.y = v1.y * (1.0f - t) + v2.y * t;
		result.z = v1.z * (1.0f - t) + v2.z * t;
		return result;
	}

private:
	// 座標補間割合
	static inline const float kInterpolationRate = 0.3f;
	// 速度掛け率
	static inline const float kVelocityBias = 0.2f;
	// 追従対象の書く方向へのカメラ移動範囲
	static inline const Rect kFollowArea = {-5.0f, 5.0f, -3.0f, 3.0f};

	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差(オフセット)
	KamataEngine::Vector3 targetOffset_ = {0, 0, -30.0f};

	// カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};

	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_;
};
