#include "PlayerBullet.h"
#include "Matrix4x4.h"

#include <cassert>

using namespace MatrixMath;
using namespace Collision;

void PlayerBullet::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	model_ = model;
	assert(camera);
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.scale_ = size; // 0.25f
	isAlive_ = false;
	lifeFrame_ = 120; // 2秒間
}

void PlayerBullet::Spawn(const KamataEngine::Vector3 startPos, const KamataEngine::Vector3 velocity) {
	worldTransform_.translation_ = startPos;
	velocity_ = velocity;
	isAlive_ = true;
	lifeFrame_ = 120; // 2秒間
}

void PlayerBullet::Update() {
	if (!isAlive_) {
		return;
	}
	// 直進
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	// 寿命カウントダウン
	lifeFrame_--;
	if (lifeFrame_ <= 0) {
		isAlive_ = false;
	}

	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファへの書き込み
	worldTransform_.TransferMatrix();
}

void PlayerBullet::Draw() {
	if (!isAlive_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);
}

PlayerBullet::~PlayerBullet() {}

bool PlayerBullet::IsCollision(AABB& aabb) {
	if (!isAlive_) {
		return false;
	}
	AABB bulletAABB;
	bulletAABB.min = {
	    worldTransform_.translation_.x - worldTransform_.scale_.x / 2.0f, 
		worldTransform_.translation_.y - worldTransform_.scale_.y / 2.0f,
	    worldTransform_.translation_.z - worldTransform_.scale_.z / 2.0f};
	bulletAABB.max = {
		worldTransform_.translation_.x + worldTransform_.scale_.x / 2.0f, 
		worldTransform_.translation_.y + worldTransform_.scale_.y / 2.0f,
	    worldTransform_.translation_.z + worldTransform_.scale_.z / 2.0f};

	if (Collision::IsCollision(bulletAABB, aabb)) {
		isAlive_ = false;
		return true;
	}
	return false;
}
