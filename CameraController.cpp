#include "CameraController.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

void CameraController::Update() {
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	targetPosition_.x = target_->GetTranslation().x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	// targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	targetPosition_.y = camera_->translation_.y;
	targetPosition_.z = target_->GetTranslation().z + targetOffset_.z + targetVelocity.z * kVelocityBias;
	// 座標補間によりゆったり追従
	camera_->translation_ = Lerp(camera_->translation_, targetPosition_, kInterpolationRate);
	// 追従対象が画面外に出ない用意補正
	camera_->translation_.x = std::fmax(camera_->translation_.x, target_->GetTranslation().x + kFollowArea.left);
	camera_->translation_.x = std::fmin(camera_->translation_.x, target_->GetTranslation().x + kFollowArea.right);
	camera_->translation_.y = std::fmax(camera_->translation_.y, target_->GetTranslation().y + kFollowArea.bottom);
	camera_->translation_.y = std::fmin(camera_->translation_.y, target_->GetTranslation().y + kFollowArea.top);

	// 移動範囲制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);
	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const Vector3& targetTranslation = target_->GetTranslation();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_.x = targetTranslation.x + targetOffset_.x;
	camera_->translation_.y = targetTranslation.y + targetOffset_.y;
	camera_->translation_.z = targetTranslation.z + targetOffset_.z;
}
