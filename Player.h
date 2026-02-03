#pragma once
#include "KamataEngine.h"
#include "Collision.h"

class MapChipField;
class PlayerBullet;

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
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, KamataEngine::Model* bulletModel);
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

	void ShootBullet();
	void UpdateBullet();
	void DrawBullet();
	
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
	bool GetIsAlive() { return isAlive_; }
	int GetLife() { return life_; }
	KamataEngine::Vector3 GetTranslation() { return worldTransform_.translation_; }
	KamataEngine::Vector3 GetVelocity() { return velocity_; }
	
	PlayerBullet* const* GetBullets() const { return bullet_; }
	static constexpr int GetMaxBullets() { return kMaxBullets; }

	/* セッター */
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	void SetTranslation(KamataEngine::Vector3 pos) { worldTransform_.translation_ = pos; }
	void SetLife(int life) { life_ = life; }

private:
	static constexpr KamataEngine::Vector3 size = {1.0f, 1.0f, 1.0f};

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 velocity_;
	bool isAlive_ = true;
	int life_ = 5;

#pragma region 定数
	/* 横移動 */
	// 加速度<単位: マス/フレーム^2>
	static inline const float kAcceleration = 0.01f;
	// 減速度<単位: マス/フレーム^2>
	static inline const float kAttenuation = 0.03f;
	// 最高速度<単位: マス/フレーム>
	static inline const float kLimitRunSpeed = 0.25f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	/* 縦移動 */
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.05f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.6f;

	/* 当たり判定 */
	// キャラクターの当たり判定サイズ
	static inline const float kTileWidth = 0.99f;
	static inline const float kTileHeight = 0.99f;
	// 接地判定のときだけ使う微調整用の下方向余白
	static inline const float kSnapFeetMargin = 0.01f; // 0.03

	/* ジャンプ */
	// 最大ジャンプ回数
	const int kMaxJumps = 2;

	// 壁すり抜け落下速度
	static inline const float kWallSliderFallSpeed = 0.25f;
	// 壁蹴りの上方向初速
	static inline const float kWallJumpUp = 0.65f;
	// 壁から離れる水平方向初速
	static inline const float kWallJumpHorizontal = 0.5f;
	// 壁コヨーテ受付フレーム
	static inline const int kWallCoyoteFrames = 8;
	static inline const int kJumpBufferFrames = 8;
	static inline const int kWallJumpCooldownFrames = 8;

#pragma endregion

#pragma region 左右旋回
	// 左右方向
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
#pragma endregion

	// 設置状態フラグ
	bool onGround_ = true;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	/* 二段ジャンプ */
	int jumpCount_ = 0; // ジャンプ回数カウント
	bool jumpHeldPrev_ = false; // 前フレームのジャンプボタン押下状態

	/* 壁蹴りジャンプ */
	bool onWall_ = false;
	int wallSide_ = 0; // -1 = 左壁 , 0 = 壁なし , 1 = 右壁
	int wallJumpCooldown_ = 0; // 連続誤発動を防ぐクールダウン(数フレーム)
	int wallCoyoteTimer_ = 0;
	int jumpBufferTimer_ = 0;

	/* 弾 */
	const int kBulletCoolFrames = 4;
	int bulletCoolTimer_ = 0;
	KamataEngine::Model* bulletModel_ = nullptr;
	static const int kMaxBullets = 100;
	PlayerBullet* bullet_[kMaxBullets];

};
