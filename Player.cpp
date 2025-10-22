#include "Player.h"
#include "Matrix4x4.h"
#include "MapChipField.h"
#include <cassert>
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MatrixMath;

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) { 
	model_ = model;
	assert(model);

	camera_ = camera;
	assert(camera);

	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.translation_ = position;

	velocity_ = {};
}

void Player::Update() {

	ApplyMoveInput();

	// 衝突判定（Y → X の順）
	CollisionMapInfo infoY{};
	infoY.velocity = {0.0f, velocity_.y, 0.0f};
	if (infoY.velocity.y > 0.0f) {
		CheckCollisionMapTop(infoY); // 天井
	} else {
		CheckCollisionMapBottom(infoY); // 地面
	}

	CollisionMapInfo infoX{};
	infoX.velocity = {velocity_.x, 0.0f, 0.0f};
	if (infoX.velocity.x < 0.0f) {
		CheckCollisionMapLeft(infoX); // 左壁
	} else if (infoX.velocity.x > 0.0f) {
		CheckCollisionMapRight(infoX); // 右壁
	}

	// 判定結果を反映して移動
	worldTransform_.translation_.x += infoX.velocity.x;
	worldTransform_.translation_.y += infoY.velocity.y;

	velocity_.x = infoX.velocity.x;
	velocity_.y = infoY.velocity.y;

	// 天井に接触している場合の処理
	ApplyCeilingCollision(infoY);

	// 壁に接触している場合の処理（ここではX方向の速度を止めている）
	if (infoX.isHitLeft || infoX.isHitRight) {
		velocity_.x = 0.0f;
	}

	// 接地状態の切り替え
	ApplyGroundState(infoY);

	// 旋回制御
	if (rotationTimer_ < rotationDuration_) {
		rotationTimer_ += 1.0f / 60.0f;
		// 自キャラの角度を設定する
		float t = std::clamp(rotationTimer_ / rotationDuration_, 0.0f, 1.0f);
		float easeT = t * t * (3.0f - 2.0f * t);
		worldTransform_.rotation_.y = std::lerp(rotationStartY_, rotationEndY_, easeT);
	}

	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファへの書き込み
	worldTransform_.TransferMatrix();
}

void Player::Draw() { 
	model_->Draw(worldTransform_, *camera_);
}

void Player::ApplyMoveInput() {
	// 移動入力

	// 左右移動操作
	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
		// 左右加速
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			// 左移動中の右入力
			if (velocity_.x < 0.0f) {
				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - kAttenuation_);
			}
			acceleration.x += kAcceleration_;
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				// 旋回開始時の回転角を記録する
				rotationStartY_ = worldTransform_.rotation_.y;
				rotationEndY_ = std::numbers::pi_v<float> / 2.0f;
				rotationTimer_ = 0.0f;
			}
		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 右移動中の左入力
			if (velocity_.x > 0.0f) {
				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - kAttenuation_);
			}
			acceleration.x -= kAcceleration_;
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				// 旋回開始時の回転角を記録する
				rotationStartY_ = worldTransform_.rotation_.y;
				rotationEndY_ = std::numbers::pi_v<float> * 3.0f / 2.0f;
				rotationTimer_ = 0.0f;
			}
		}
		// 加速/減速
		velocity_.x += acceleration.x;
		velocity_.y += acceleration.y;
		velocity_.z += acceleration.z;

		// 最大速度制限
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed_, kLimitRunSpeed_);

	} else {
		// 非入力時は移動減衰をかける
		velocity_.x *= (1.0f - kAttenuation_);
	}

	bool jumpHeldNow = Input::GetInstance()->PushKey(DIK_UP);
	bool jumpPressed = (jumpHeldNow && !jumpHeldPrev_);

	if (jumpPressed && (onGround_|| jumpCount_ < kMaxJumps_)) {
		// ジャンプ初速
		velocity_.y = kJumpAcceleration_;
		onGround_ = false;
		jumpCount_++;
	}
	// 空中なら重力
	if (!onGround_) {
		// 落下速度
		velocity_.y += -kGravityAcceleration_;
		// 落下速度制限
		velocity_.y = std::fmaxf(velocity_.y, -kLimitFallSpeed_);
	}

	jumpHeldPrev_ = jumpHeldNow;
}

void Player::CheckCollisionMapTop(CollisionMapInfo& info) {

	// 上昇あり？
	if (info.velocity.y <= 0) {
		return;
	}

	// 移動後の4つ角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 center = {worldTransform_.translation_.x + info.velocity.x, worldTransform_.translation_.y + info.velocity.y, worldTransform_.translation_.z + info.velocity.z};
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	// 真上の当たり判定を行う
	bool hit = false;
	float topY = 0.0f;

	// 左上
	{
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			topY = rect.bottom;
			// topY = std::fmin(topY, rect.bottom);
			hit = true;
		}
	}

	// 右上
	{
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// どちらか下のほうで止める
			topY = rect.bottom;
			// topY = std::fmin(topY, rect.bottom);
			hit = true;
		}
	}

	if (hit) {
		float epsilon = 0.01f; // 壁にめり込むのを防ぐための、ごくわずかな隙間
		// worldTransform_.translation_.y = topY - kHeight / 2.0f;
		// worldTransform_.translation_.y = topY - kHeight / 2.0f;
		worldTransform_.translation_.y = topY - kHeight / 2.0f - epsilon;
		velocity_.y = 0.0f;
		info.velocity.y = 0.0f;
		info.isHitTop = true;
	}
}

void Player::CheckCollisionMapBottom(CollisionMapInfo& info) {
	bool hit = false;
	bool isSnapCheck = (!onGround_ && info.velocity.y >= 0.0f);

	if (!isSnapCheck && info.velocity.y >= 0.0f) {
		return;
	}

	// 移動後の角座標を計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 center = {worldTransform_.translation_.x + info.velocity.x, worldTransform_.translation_.y + info.velocity.y, worldTransform_.translation_.z + info.velocity.z};
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));

		// 吸着判定用のY方向の微小オフセットを追加
		if (isSnapCheck) {
			positionsNew[i].y -= kSnapFeetMargin;
		}
	}

	float bottomY = 0.0f;

	// 左下の判定
	{
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			bottomY = rect.top;
			// bottomY = std::fmax(bottomY, rect.top + kBlank);
			hit = true;
		}
	}

	// 右下の判定
	{
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			bottomY = rect.top;
			// bottomY = std::fmax(bottomY, rect.top + kBlank);
			hit = true;
		}
	}

	if (hit) {
		worldTransform_.translation_.y = bottomY + kHeight / 2.0f;
		velocity_.y = 0.0f;
		info.velocity.y = 0.0f;
		info.isHitBottom = true;
	}
}

void Player::CheckCollisionMapLeft(CollisionMapInfo& info) {
	if (info.velocity.x >= 0)
		return;

	// 角座標の算出
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 center = {worldTransform_.translation_.x + info.velocity.x, worldTransform_.translation_.y + info.velocity.y, worldTransform_.translation_.z + info.velocity.z};
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	bool hit = false;
	float leftX = 0.0f;

	// 左上
	{
		auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			auto rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			leftX = rect.right;
			hit = true;
			DebugText::GetInstance()->ConsolePrintf("LeftHit at (%.2f,%.2f) tile(%u,%u)\n", positionsNew[kLeftTop].x, positionsNew[kLeftTop].y, indexSet.xIndex, indexSet.yIndex);
		}
	}
	// 左下
	{
		auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			auto rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// leftX = std::fmax(leftX, rect.right);
			leftX = rect.right;
			hit = true;
		}
	}

	if (hit) {
		// worldTransform_.translation_.x = leftX + kWidth / 2.0f;
		velocity_.x = 0.0f;
		info.velocity.x = 0.0f;
		info.isHitLeft = true;
	}
}

void Player::CheckCollisionMapRight(CollisionMapInfo& info) {
	if (info.velocity.x <= 0)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 center = {worldTransform_.translation_.x + info.velocity.x, worldTransform_.translation_.y + info.velocity.y, worldTransform_.translation_.z + info.velocity.z};
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	bool hit = false;
	float rightX = 0.0f;

	// 右上
	{
		auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			auto rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			rightX = rect.left;
			hit = true;
		}
	}
	// 右下
	{
		auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			auto rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// rightX = std::fmin(rightX, rect.left);
			rightX = rect.left;
			hit = true;
		}
	}

	if (hit) {
		// worldTransform_.translation_.x = rightX - kWidth / 2.0f;
		velocity_.x = 0.0f;
		info.velocity.x = 0.0f;
		info.isHitRight = true;
	}
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // 右下
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // 左下
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // 右上
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, // 左上
	};

	Vector3 result = {center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};

	return result;
}

void Player::ApplyCollisionResult(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.velocity.x;
	worldTransform_.translation_.y += info.velocity.y;
	worldTransform_.translation_.z += info.velocity.z;
}

void Player::ApplyCeilingCollision(const CollisionMapInfo& info) {
	if (info.isHitTop) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
}

void Player::ApplyGroundState(const CollisionMapInfo& info) {
	// 今が地面にいる状態かどうか
	if (onGround_) {
		// 地面にいたけどジャンプ開始したら空中へ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下開始していたら、地面から離れたとみなして空中へ
			MapChipType mapChipType;
			// bool hit = false;

			// 左下の角で判定
			mapChipType = mapChipField_->GetMapChipTypeByIndex(
			    mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLeftBottom)).xIndex,
			    mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLeftBottom)).yIndex);
			if (mapChipType != MapChipType::kBlock) {
				// 右下の角で判定
				mapChipType = mapChipField_->GetMapChipTypeByIndex(
				    mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kRightBottom)).xIndex,
				    mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kRightBottom)).yIndex);
				if (mapChipType != MapChipType::kBlock) {
					// 両方ブロックじゃなければ落下中とみなす
					onGround_ = false;
				}
			}
		}
	} else {
		// 空中にいて、地面に当たったら設置状態に
		if (info.isHitBottom) {
			onGround_ = true;
			jumpCount_ = 0;
		}
	}
}