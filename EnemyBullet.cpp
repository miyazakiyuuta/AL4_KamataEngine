#include "EnemyBullet.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <cassert>
#include <numbers>

using namespace KamataEngine;
using namespace MatrixMath;
using namespace Vector3Math;


void EnemyBullet::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	model_ = model;
	assert(camera);
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.scale_ = size;
	speed_ = 0.1f;
	turnSpeed_ = 0.03f;
	isAlive_ = false;
	lifeFrame_ = 10 * 60;
}

void EnemyBullet::Spawn(const KamataEngine::Vector3 startPos) {
	worldTransform_.translation_ = startPos;
	if (isTracking_) {
		worldTransform_.rotation_.z = std::numbers::pi_v<float> / 2.0f; // 上向きに初期化
	} else {
		worldTransform_.rotation_.z = 0.0f;
	}
	isAlive_ = true;
	lifeFrame_ = 10 * 60;
}
  
void EnemyBullet::Update() {
	if (!isAlive_) {
		return;
	}
	
	if (isTracking_) { // 追尾弾
		Tracking();
	} else { // 追尾しない処理
		velocity_.x = -0.25f;
		worldTransform_.translation_.x += velocity_.x;
	}

	if (worldTransform_.translation_.y < 21.0f) {
		isAlive_ = false;
	}

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

void EnemyBullet::Draw() {
	if (!isAlive_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);
}

bool EnemyBullet::IsCollision(AABB& aabb) {
	if (!isAlive_) {
		return false;
	}
	AABB bulletAABB;
	bulletAABB.min = {
	    worldTransform_.translation_.x - worldTransform_.scale_.x / 2.0f, worldTransform_.translation_.y - worldTransform_.scale_.y / 2.0f,
	    worldTransform_.translation_.z - worldTransform_.scale_.z / 2.0f};
	bulletAABB.max = {
	    worldTransform_.translation_.x + worldTransform_.scale_.x / 2.0f, worldTransform_.translation_.y + worldTransform_.scale_.y / 2.0f,
	    worldTransform_.translation_.z + worldTransform_.scale_.z / 2.0f};

	if (Collision::IsCollision(bulletAABB, aabb)) {
		isAlive_ = false;
		return true;
	}
	return false;
}

void EnemyBullet::Tracking() {
	Vector3 toTarget_ = {targetPos_.x - worldTransform_.translation_.x, targetPos_.y - worldTransform_.translation_.y, 0};
	toTarget_ = Normalize(toTarget_);

	Vector2 forward = {std::cos(worldTransform_.rotation_.z), std::sin(worldTransform_.rotation_.z)};

	// 左右判定
	float crossZ = forward.x * toTarget_.y - forward.y * toTarget_.x;
	if (crossZ > 0.0f) {
		// 右回転
		worldTransform_.rotation_.z += turnSpeed_;
	} else if (crossZ < 0.0f) {
		// 左回転
		worldTransform_.rotation_.z -= turnSpeed_;
	}
	// 回転後の正面を更新
	forward = {std::cos(worldTransform_.rotation_.z), std::sin(worldTransform_.rotation_.z)};
	// 速度に正面方向を掛けて移動量を計算
	velocity_.x = forward.x * speed_;
	velocity_.y = forward.y * speed_;

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
}
