#pragma once
#include "MyMath.h"
#include<cstdint>


// 球体の描画関数
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);

//
bool IsCollision(const Sphere& s1, const Sphere& s2);