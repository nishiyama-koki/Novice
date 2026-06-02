#include "Sphere.h"
#include <Novice.h>
#include <cmath>

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewPlojectionMatrix, const Matrix4x4& viewPortMatrix,uint32_t color) {
	const uint32_t kSubdivision = 20; // 分割数
	const float kPi = 3.1415926535f;
	const float kLonEvery = (2.0f * kPi) / float(kSubdivision); // 経度分割1つ分の角度
	const float kLatEvery = kPi / float(kSubdivision);          // 緯度分割1つ分の角度

	// 緯度の方向に分割 -π/2~π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -kPi / 2.0f + kLatEvery * float(latIndex); // 緯度

		// 経度の方向に分割 0~2π
		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
			float lon = float(lonIndex) * kLonEvery; // 経度

			// world座標系でのa,b,c,を求める
			Vector3 a{};
			a.x = sphere.center.x + sphere.radius * cosf(lat) * cosf(lon);
			a.y = sphere.center.y + sphere.radius * sinf(lat);
			a.z = sphere.center.z + sphere.radius * cosf(lat) * sinf(lon);

			Vector3 b{};
			b.x = sphere.center.x + sphere.radius * cosf(lat + kLatEvery) * cosf(lon);
			b.y = sphere.center.y + sphere.radius * sinf(lat + kLatEvery);
			b.z = sphere.center.z + sphere.radius * cosf(lat + kLatEvery) * sinf(lon);

			Vector3 c{};
			c.x = sphere.center.x + sphere.radius * cosf(lat) * cosf(lon + kLonEvery);
			c.y = sphere.center.y + sphere.radius * sinf(lat);
			c.z = sphere.center.z + sphere.radius * cosf(lat) * sinf(lon + kLonEvery);

			// a,b,cをScreen座標系まで変換
			Vector3 screenA = Transform(Transform(a, viewPlojectionMatrix), viewPortMatrix);
			Vector3 screenB = Transform(Transform(b, viewPlojectionMatrix), viewPortMatrix);
			Vector3 screenC = Transform(Transform(c, viewPlojectionMatrix), viewPortMatrix);

			// ab,bcで線を引く
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(Subtract(s2.center, s1.center));
	if (distance < s1.radius + s2.radius) {
		return true;
	}
	return false;
}