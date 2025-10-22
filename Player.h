#pragma once
#include "KamataEngine.h"

class MapChipField;

enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上
	kNumCorner    // 要素数
};

struct CollisionMapInfo {
	bool isHitTop = false;
	bool isHitBottom = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	KamataEngine::Vector3 velocity;
};

class Player {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	void ApplyMoveInput();

	// マップ判定関数
	void CheckCollisionMapTop(CollisionMapInfo& info);
	void CheckCollisionMapBottom(CollisionMapInfo& info);
	void CheckCollisionMapLeft(CollisionMapInfo& info);
	void CheckCollisionMapRight(CollisionMapInfo& info);

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	void ApplyCollisionResult(const CollisionMapInfo& info);
	void ApplyCeilingCollision(const CollisionMapInfo& info);
	void ApplyGroundState(const CollisionMapInfo& info);

	/* セッター */
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Vector3 velocity_;

	static inline const float kAcceleration_ = 0.01f;
	static inline const float kAttenuation_ = 0.03f;
	static inline const float kLimitRunSpeed_ = 2.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration_ = 0.05f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed_ = 1.0f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration_ = 0.6f;
	// キャラクターの当たり判定サイズ
	/*static inline const float kWidth = 0.99f;
	static inline const float kHeight = 0.99f;*/
	static inline const float kWidth = 1.9f;
	static inline const float kHeight = 1.9f;

	static inline const float kAttenuationLanding = 0.2f;
	// 接地判定のときだけ使う微調整用の下方向余白
	static inline const float kSnapFeetMargin = 0.01f; // 0.03

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回アニメーション用
	float rotationStartY_ = 0.0f;   // 開始角度
	float rotationEndY_ = 0.0f;     // 終了角度
	float rotationTimer_ = 0.0f;    // 経過時間
	float rotationDuration_ = 0.2f; // 補間にかける時間（秒数）

	// 設置状態フラグ
	bool onGround_ = true;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
};
