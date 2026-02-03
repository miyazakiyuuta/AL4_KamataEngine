#include "Enemy.h"
#include "Matrix4x4.h"
#include "WorldTransformUtility.h"

using namespace KamataEngine;
using namespace MatrixMath;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.scale_ = size;
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -3.14f; // 180度回転

	basePos_ = position;
	startPos_ = position;
	endPos_ = {};

	stateElapsedTime_ = 0.0f;
	stateDuration_ = 1.0f;

	hp_ = kMaxHp;
	shootInterval_ = 30;
	maxShot_ = 6;
	for (uint32_t i = 0; i < kMaxBullets; ++i) {
		bullet_[i] = new EnemyBullet();
		bullet_[i]->Initialize(model, camera);
	}
	UpdateWorldTransform(worldTransform_);
}

void Enemy::Update() {
	if (hp_ <= 0) {
		isAlive_ = false;
		for (uint32_t i = 0; i < kMaxBullets; ++i) {
			bullet_[i]->Kill();
		}
	}
	if (!isAlive_) { // 倒されたときの処理
		float downSpeed = 0.05f;
		worldTransform_.rotation_.x -= downSpeed;
		worldTransform_.translation_.y -= downSpeed;
		worldTransform_.translation_.z += downSpeed;
		UpdateWorldTransform(worldTransform_);
	}
	if (!isAttack_) {
		UpdateWorldTransform(worldTransform_);
		return;
	}

	// (isAlive && isAttack)ならここより先に到達

	switch (attackState_) {
	case AttackState::kWait: // 5秒
		stateElapsedTime_ += 1.0f / 60.0f;
		if (stateElapsedTime_ >= stateDuration_) {
			stateElapsedTime_ = 0.0f; // タイマーリセット
			stateDuration_ = 5.0f;    // 次の攻撃ステートの時間
			if (hp_ <= kMaxHp * 2.0f / 5.0f) {
				maxShot_ = 3;
				if (hp_ <= kMaxHp * 1.0f / 5.0f) {
					maxShot_ = 6;
					if (hp_ <= kMaxHp * 0.5f / 5.0f) {
						maxShot_ = 9;
					}
				}
				attackState_ = AttackState::kHomingShot;
			} else {
				maxShot_ = 3;
				if (hp_ <= kMaxHp * 4.0f / 5.0f) {
					maxShot_ = 6;
					if (hp_ <= kMaxHp * 3.0f / 5.0f) {
						maxShot_ = 9;
					}
				}
				attackState_ = AttackState::kStraightShot;
			}
		}
		break;
	case AttackState::kStraightShot:
		AttackStraightShot();
		stateElapsedTime_ += 1.0f / 60.0f;
		if (stateElapsedTime_ >= stateDuration_) {
			stateElapsedTime_ = 0.0f;
			attackState_ = AttackState::kDropSmash;
		}
		break;
	case AttackState::kHomingShot:
		AttackHomingShot();
		stateElapsedTime_ += 1.0f / 60.0f;
		if (stateElapsedTime_ >= stateDuration_) {
			stateElapsedTime_ = 0.0f;
			attackState_ = AttackState::kDropSmash;
		}
		break;
	case AttackState::kDropSmash: // 落下攻撃(タイマーで攻撃ステート変更はしない)
		stateElapsedTime_ = 0.0f;
		AttackDropSmash();
		if (stateElapsedTime_ != 0.0f) {
			stateElapsedTime_ = 0.0f;
			stateDuration_ = 1.0f; // 5秒
			attackState_ = AttackState::kWait;
		}
		break;
	}

	for (uint32_t i = 0; i < kMaxBullets; ++i) {
		bullet_[i]->Update();
	}

	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() {
	for (uint32_t i = 0; i < kMaxBullets; ++i) {
		bullet_[i]->Draw();
	}
	model_->Draw(worldTransform_, *camera_);
}

bool Enemy::IsBulletCollision(AABB& aabb) {
	for (int i = 0; i < kMaxBullets; i++) {
		if (bullet_[i]->IsCollision(aabb)) {
			return true;
		}
	}
	return false;
}

void Enemy::AttackStraightShot() {
	shootTimer_++;
	shootInterval_ = 60;
	if (shotCount_ > 0) {
		shootInterval_ = 5;
	}
	if (shootTimer_ >= shootInterval_) {
		shootTimer_ = 0;
		for (uint32_t i = 0; i < kMaxBullets; ++i) {
			if (!bullet_[i]->GetIsAlive()) {

				Vector3 spawnPos = worldTransform_.translation_;
				static const float kOffsetX[3] = {0.0f, -1.0f, 1.0f};
				spawnPos.x = worldTransform_.translation_.x + kOffsetX[shotCount_ % 3];
				spawnPos.y = worldTransform_.translation_.y + kOffsetX[shotCount_ % 3];
				bullet_[i]->Spawn(spawnPos);

				++shotCount_;
				if (shotCount_ % 3 == 0) { // 1フレーム３発まで出す
					if (shotCount_ >= maxShot_) {
						shotCount_ = 0;
					}
					break;
				}
			}
		}
	}
}

void Enemy::AttackHomingShot() {
	shootTimer_++;
	shootInterval_ = 30;
	if (shotCount_ != 0) {
		shootInterval_ = 10;
	}
	if (shootTimer_ >= shootInterval_) {
		shootTimer_ = 0;
		for (uint32_t i = 0; i < kMaxBullets; ++i) {
			if (!bullet_[i]->GetIsAlive()) {
				Vector3 spawnPos = worldTransform_.translation_;
				static const float kOffsetX[3] = {0.0f, -1.0f, 1.0f};
				spawnPos.x = worldTransform_.translation_.x + kOffsetX[shotCount_ % 3];
				spawnPos.y = worldTransform_.translation_.y + 1.5f;
				bullet_[i]->Spawn(spawnPos);
				bullet_[i]->SetIsTracking(true);
				shotCount_++;
				if (shotCount_ % 3 == 0) { // 1フレーム３発まで出す
					if (shotCount_ >= maxShot_) {
						shotCount_ = 0;
					}
					break;
				}
			}
		}
	}
}

void Enemy::AttackDropSmash() {
	switch (dropPhase_) {
	case DropSmashPhase::kMoveAboveTarget: { // ターゲットの頭上に移動

		endPos_.x = targetPosX_;        // ターゲットの頭上を追う
		endPos_.y = basePos_.y + 12.0f; // 地面から6マス上
		endPos_.z = basePos_.z;

		t_ += (1.0f / 60.0f) / 0.5f; // (1/60)/(s)
		float easedT = 1.0f - powf(1.0f - t_, 5);
		worldTransform_.translation_ = Lerp(startPos_, endPos_, easedT);
		if (t_ >= 1.0f) {
			t_ = 0.0f;
			dropPhase_ = DropSmashPhase::kHover;
		}

		break;
	}
	case DropSmashPhase::kHover: // 移動後数秒停止
		// 動かずに数秒停止

		t_ += (1.0f / 60.0f) / 0.5f;
		if (t_ >= 1.0f) {
			t_ = 0.0f;
			dropPhase_ = DropSmashPhase::kFall;
		}

		break;
	case DropSmashPhase::kFall: { // 落下攻撃(ドッスン)

		t_ += (1.0f / 60.0f) / 0.25f;

		float easedT = t_ * t_ * t_ * t_;
		Vector3 v1 = endPos_;
		Vector3 v2 = {endPos_.x, basePos_.y, basePos_.z};
		worldTransform_.translation_ = Lerp(v1, v2, easedT);

		if (t_ >= 1.0f) {
			t_ = 0.0f;
			dropPhase_ = DropSmashPhase::kRecover;
		}

		break;
	}
	case DropSmashPhase::kRecover: // 落下攻撃後数秒停止
		t_ += (1.0f / 60.0f) / 1.0f;
		if (t_ >= 1.0f) {
			t_ = 0.0f;
			startPos_ = worldTransform_.translation_;
			dropLoopCount_++; // 落下攻撃回数を追加
			if (dropLoopCount_ >= 3) {
				dropPhase_ = DropSmashPhase::kReturnToPos;
			} else {
				dropPhase_ = DropSmashPhase::kMoveAboveTarget;
				startPos_ = worldTransform_.translation_;
			}
		}
		break;
	case DropSmashPhase::kReturnToPos: {
		t_ += (1.0f / 60.0f) / 0.5f;

		Vector3 p0 = startPos_;                                                           // start
		Vector3 p1 = {(startPos_.x + basePos_.x) / 2.0f, basePos_.y + 15.0f, basePos_.z}; // 制御点
		Vector3 p2 = basePos_;                                                            // end

		float easedT = 1.0f - powf(1.0f - t_, 3.0f);
		Vector3 a = Lerp(p0, p1, easedT);
		easedT = t_ * t_ * t_;
		Vector3 b = Lerp(p1, p2, easedT);
		worldTransform_.translation_ = Lerp(a, b, t_);

		if (t_ >= 1.0f) {
			t_ = 0.0f;
			dropLoopCount_ = 0;
			startPos_ = basePos_;
			dropPhase_ = DropSmashPhase::kMoveAboveTarget;
			stateElapsedTime_ = 1.0f; // 攻撃フェーズの変更のため
		}

		break;
	}
	}
}