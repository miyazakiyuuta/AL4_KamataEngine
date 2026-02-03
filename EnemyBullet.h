#pragma once
#include "Collision.h"
#include "KamataEngine.h"

class EnemyBullet {
public:
	EnemyBullet() = default;
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Spawn(const KamataEngine::Vector3 startPos);
	void Update();
	void Draw();
	void Kill() { isAlive_ = false; }
	bool IsCollision(AABB& aabb);
	/* ゲッター */
	bool GetIsAlive() { return isAlive_; }
	AABB GetAABB() {
		AABB aabb;
		aabb.min = {
		    worldTransform_.translation_.x - size.x / 2.0f,
		    worldTransform_.translation_.y - size.y / 2.0f,
		    worldTransform_.translation_.z - size.z / 2.0f,
		};
		aabb.max = {
		    worldTransform_.translation_.x + size.x / 2.0f,
		    worldTransform_.translation_.y + size.y / 2.0f,
		    worldTransform_.translation_.z + size.z / 2.0f,
		};
		return aabb;
	}

	void SetTargetPos(KamataEngine::Vector3 pos) { targetPos_ = pos; }
	void SetIsTracking(bool tracking) { isTracking_ = tracking; }

private:
	void Tracking();

private:
	static constexpr KamataEngine::Vector3 size = {0.25f, 0.25f, 0.25f};
	// static constexpr KamataEngine::Vector3 size = {0.5f, 0.5f, 0.5f};

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 velocity_;
	float speed_;
	float turnSpeed_;
	bool isAlive_ = false;
	int lifeFrame_ = 0;

	KamataEngine::Vector3 targetPos_;

	bool isTracking_ = false;
};
