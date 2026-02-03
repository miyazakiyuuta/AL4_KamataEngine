#pragma once
#include "KamataEngine.h"
#include "Collision.h"

class MapChipField;

class PlayerBullet {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Spawn(const KamataEngine::Vector3 startPos, const KamataEngine::Vector3 velocity);
	void Update();
	void Draw();
	void Finalize();
	~PlayerBullet();

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

private:
	static constexpr KamataEngine::Vector3 size = {0.3f, 0.3f, 0.3f};

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 velocity_;
	MapChipField* map_ = nullptr;
	bool isAlive_ = false;
	int lifeFrame_ = 0;
};
