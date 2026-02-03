#include "Collision.h"

bool Collision::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && // x軸
	    aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y && // y軸
	    aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) { // z軸
		return true;
	}
	return false;
}
