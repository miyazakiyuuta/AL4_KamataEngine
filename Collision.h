#pragma once
#include "KamataEngine.h"

struct AABB {
	KamataEngine::Vector3 min; // 最小点（x,y,z）
	KamataEngine::Vector3 max; // 最大点（x,y,z）
};