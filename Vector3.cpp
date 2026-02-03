#include "Vector3.h"

using namespace KamataEngine;

float Vector3Math::Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector3Math::Length(const KamataEngine::Vector3& v) { 
	return sqrtf(Dot(v, v));
}

KamataEngine::Vector3 Vector3Math::Normalize(const KamataEngine::Vector3& v) { 
	Vector3 result = {};
	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	result.z = v.z / Length(v);
	return result;
}
