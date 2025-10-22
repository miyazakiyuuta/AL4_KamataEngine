#pragma once
#include "KamataEngine.h"

namespace MatrixMath {

// 積
KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
// 逆行列
KamataEngine::Matrix4x4 Inverse(const KamataEngine::Matrix4x4& m);
// 転置行列
KamataEngine::Matrix4x4 Transpose(const KamataEngine::Matrix4x4& m);
// 単位行列の作成
KamataEngine::Matrix4x4 MakeIdentity4x4();
// 平行移動行列
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);
// 拡大縮小行列
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
// 座標変換
KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);
// X軸回転行列
KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
// Y軸回転行列
KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
// Z軸回転行列
KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);
// 3次元アフィン変換行列
KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);
// cotangent(tangentの逆数)
float cot(float radian);
// 透視投影行列
KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
// 正射影行列
KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
// ビューポート変換行列
KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
} // namespace MatrixMath