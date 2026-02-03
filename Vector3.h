#pragma once
#include "KamataEngine.h"

namespace Vector3Math {
// 内積
float Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
// 長さ(ノルム)
float Length(const KamataEngine::Vector3& v);
// 正規化
KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v);
}
