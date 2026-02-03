#pragma once
#include "Collision.h"
#include "EnemyBullet.h"
#include "KamataEngine.h"
#include <vector>

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	void Finalize();

	bool IsBulletCollision(AABB& aabb);

	/* ゲッター */
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
	int GetMaxHp() { return kMaxHp; }
	int GetHp() { return hp_; }
	bool GetIsAlive() { return isAlive_; }
	KamataEngine::Vector3 GetPosition() { return worldTransform_.translation_; }
	EnemyBullet* const* GetBullets() const { return bullet_; }
	static constexpr int GetMaxBullets() { return kMaxBullets; }

	/* セッター */
	void SetHp(int hp) { hp_ = hp; }
	void SetPos(KamataEngine::Vector3 pos) { worldTransform_.translation_ = pos; }
	void SetBulletTargetPos(KamataEngine::Vector3 pos) {
		for (uint32_t i = 0; i < kMaxBullets; ++i) {
			bullet_[i]->SetTargetPos(pos);
		}
	}
	void SetIsAttack(bool isAttack) { isAttack_ = isAttack; }
	void SetTargetPosX(float posX) { targetPosX_ = posX; }

private:

	void AttackStraightShot();
	void AttackHomingShot();
	void AttackDropSmash();

	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3 v1, const KamataEngine::Vector3& v2, float t) {
		KamataEngine::Vector3 result;
		result.x = v1.x * (1.0f - t) + v2.x * t;
		result.y = v1.y * (1.0f - t) + v2.y * t;
		result.z = v1.z * (1.0f - t) + v2.z * t;
		return result;
	}

private:
	enum class AttackState {
		kWait,
		kStraightShot,
		kHomingShot,
		kDropSmash,
	};
	AttackState attackState_ = AttackState::kWait;
	float stateElapsedTime_; // タイマー
	float stateDuration_; // 時間

	// 落下攻撃
	enum class DropSmashPhase {
		kMoveAboveTarget, // 頭上へ移動
		kHover,           // 停止（溜め）
		kFall,            // 落下
		kRecover,         // 着地後停止
		kReturnToPos,
	};
	DropSmashPhase dropPhase_;
	float dropPhaseElapsedTime_;
	int dropLoopCount_ = 0;
	float targetPosX_;
	float t_;
	KamataEngine::Vector3 basePos_;
	KamataEngine::Vector3 startPos_;
	KamataEngine::Vector3 endPos_;

	static constexpr KamataEngine::Vector3 size = {4.0f, 4.0f, 4.0f};

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	bool isAttack_ = false; // 攻撃中か否か(プレイヤーとバトルしているか)
	bool isAlive_ = true;
	const int kMaxHp = 500;
	int hp_;

	// 弾関連
	int shootInterval_; // 次の弾が打たれるまでの時間(!timer)
	int shootTimer_ = 0; // 次の弾までの間隔タイマー
	int maxShot_; // 1ターンで打つ最大弾数
	int shotCount_ = 0; // 現在のターンで打っている弾カウント
	static constexpr uint32_t kMaxBullets = 512; // 64 // 弾の最大数
	EnemyBullet* bullet_[kMaxBullets];

	

};
