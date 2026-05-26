#pragma once
#include "MyMath.h"

struct Sphere {
	Vector3 center; // 中心点
	float radius;   // 半径
};

// 球体の描画関数
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewPlojectionMatrix, const Matrix4x4& viewPortMatrix);