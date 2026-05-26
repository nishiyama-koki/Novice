#pragma once
#include "MyMath.h"
#include<cstdint>

struct Sphere {
	Vector3 center; // 中心点
	float radius;   // 半径
};

// 球体の描画関数
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);