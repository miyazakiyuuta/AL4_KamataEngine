#include "WorldTransformUtility.h"
#include "Matrix4x4.h"

using namespace KamataEngine;
using namespace MatrixMath;

/// <summary>
/// 行列を計算・転送する
/// </summary>
/// <param name="worldTransform"></param>
void UpdateWorldTransform(KamataEngine::WorldTransform& worldTransform) {
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	// 定数バッファへの書き込み
	worldTransform.TransferMatrix();
}
