#include "MyMath.h"
#include "Sphere.h"
#include <Novice.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>

const char kWindowTitle[] = "GC2A_ニシヤマ_コウキ";

// グリッド描画関数
void DrawGrid(const Matrix4x4& viewPlojectionMatrix4x4, const Matrix4x4& viewPortMatrix4x4) {
	const float kGridHalfWidth = 2.0f;                                   // グリッドの半分の幅
	const uint32_t kSubdivision = 10;                                    // グリッドの分割数
	const float kGridEvery = (kGridHalfWidth * 2) / float(kSubdivision); // 1つ分の長さ
	// 奥から手前への線を順々にひいていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + xIndex * kGridEvery;
		Vector3 start = Transform({x, 0.0f, -kGridHalfWidth}, viewPlojectionMatrix4x4);
		Vector3 end = Transform({x, 0.0f, kGridHalfWidth}, viewPlojectionMatrix4x4);
		start = Transform(start, viewPortMatrix4x4);
		end = Transform(end, viewPortMatrix4x4);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), 0xFFFFFF77);
	}
	// 左から右への線を順々にひいていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + zIndex * kGridEvery;
		Vector3 start = Transform({-kGridHalfWidth, 0.0f, z}, viewPlojectionMatrix4x4);
		Vector3 end = Transform({kGridHalfWidth, 0.0f, z}, viewPlojectionMatrix4x4);
		start = Transform(start, viewPortMatrix4x4);
		end = Transform(end, viewPortMatrix4x4);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), 0xFFFFFF77);
	}
} // AABBの描画
void DrawAABB(const AABB& aabb, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
Vector3 vertices[8];
vertices[0] = {aabb.min.x, aabb.min.y, aabb.min.z};
vertices[1] = {aabb.max.x, aabb.min.y, aabb.min.z};
vertices[2] = {aabb.max.x, aabb.min.y, aabb.max.z};
vertices[3] = {aabb.min.x, aabb.min.y, aabb.max.z};
vertices[4] = {aabb.min.x, aabb.max.y, aabb.min.z};
vertices[5] = {aabb.max.x, aabb.max.y, aabb.min.z};
vertices[6] = {aabb.max.x, aabb.max.y, aabb.max.z};
vertices[7] = {aabb.min.x, aabb.max.y, aabb.max.z};

Matrix4x4 worldMatrix[8];
Matrix4x4 worldViewProjectionMatrix[8];
Vector3 ndcVertex[8];
Vector3 screenVertices[8];

for (int i = 0; i < 8; i++) {
	worldMatrix[i] = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, vertices[i]);
	worldViewProjectionMatrix[i] = Multiply(worldMatrix[i], viewProjectionMatrix);
	ndcVertex[i] = Transform(Vector3{}, worldViewProjectionMatrix[i]);
	screenVertices[i] = Transform(ndcVertex[i], viewportMatrix);
}

Novice::DrawLine((int)screenVertices[0].x, (int)screenVertices[0].y, (int)screenVertices[1].x, (int)screenVertices[1].y, color);
Novice::DrawLine((int)screenVertices[1].x, (int)screenVertices[1].y, (int)screenVertices[2].x, (int)screenVertices[2].y, color);
Novice::DrawLine((int)screenVertices[2].x, (int)screenVertices[2].y, (int)screenVertices[3].x, (int)screenVertices[3].y, color);
Novice::DrawLine((int)screenVertices[3].x, (int)screenVertices[3].y, (int)screenVertices[0].x, (int)screenVertices[0].y, color);
Novice::DrawLine((int)screenVertices[4].x, (int)screenVertices[4].y, (int)screenVertices[5].x, (int)screenVertices[5].y, color);
Novice::DrawLine((int)screenVertices[5].x, (int)screenVertices[5].y, (int)screenVertices[6].x, (int)screenVertices[6].y, color);
Novice::DrawLine((int)screenVertices[6].x, (int)screenVertices[6].y, (int)screenVertices[7].x, (int)screenVertices[7].y, color);
Novice::DrawLine((int)screenVertices[7].x, (int)screenVertices[7].y, (int)screenVertices[4].x, (int)screenVertices[4].y, color);
Novice::DrawLine((int)screenVertices[1].x, (int)screenVertices[1].y, (int)screenVertices[5].x, (int)screenVertices[5].y, color);
Novice::DrawLine((int)screenVertices[2].x, (int)screenVertices[2].y, (int)screenVertices[6].x, (int)screenVertices[6].y, color);
Novice::DrawLine((int)screenVertices[3].x, (int)screenVertices[3].y, (int)screenVertices[7].x, (int)screenVertices[7].y, color);
Novice::DrawLine((int)screenVertices[0].x, (int)screenVertices[0].y, (int)screenVertices[4].x, (int)screenVertices[4].y, color);
}
bool AABBToSphereIsCollision(const AABB& aabb, const Sphere& sphere) {
	// 最近接点を求める
	Vector3 closestPoint{std::clamp(sphere.center.x, aabb.min.x, aabb.max.x), std::clamp(sphere.center.y, aabb.min.y, aabb.max.y), std::clamp(sphere.center.z, aabb.min.z, aabb.max.z)};

	// 最近接点と球の中心との距離を求める
	float distance = Length(Subtract(closestPoint, sphere.center));

	// 距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	} else {
		return false;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

// ライブラリの初期化
Novice::Initialize(kWindowTitle, 1280, 720);

// キー入力結果を受け取る箱
char keys[256] = {0};
char preKeys[256] = {0};

Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
Vector3 cameraRotate{0.26f, 0.0f, 0.0f};

AABB aabb1 = {
    .min{-0.5f, -0.5f, -0.5f},
    .max{0.0f,  0.0f,  0.0f },
};

uint32_t aabb1Color = WHITE;

Sphere sphere = {
    {0.0f, 2.0f, 0.0f},
    1.0f,
};

uint32_t sphereColor = WHITE;

// ウィンドウの×ボタンが押されるまでループ
while (Novice::ProcessMessage() == 0) {
	// フレームの開始
	Novice::BeginFrame();

	// キー入力を受け取る
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

	///
	/// ↓更新処理ここから
	///

	ImGui::Begin("Window");

	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("Camera Translate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("Camera Rotate", &cameraRotate.x, 0.01f);
	}

	if (ImGui::CollapsingHeader("AABB1")) {
		ImGui::DragFloat3("AABB1.min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("AABB1.max", &aabb1.max.x, 0.01f);
	}

	if (ImGui::CollapsingHeader("Sphere")) {
		ImGui::DragFloat3("Sphere Center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere Radius", &sphere.radius, 0.01f);
	}

	ImGui::End();

	if (AABBToSphereIsCollision(aabb1, sphere)) {
		aabb1Color = RED;
		sphereColor = RED;
	} else {
		aabb1Color = WHITE;
		sphereColor = WHITE;
	}

	Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraTranslate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

	///
	/// ↑更新処理ここまで
	///

	///
	/// ↓描画処理ここから
	///

	DrawGrid(viewProjectionMatrix, viewportMatrix);
	DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, aabb1Color);
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, sphereColor);

	///
	/// ↑描画処理ここまで
	///

	// フレームの終了
	Novice::EndFrame();

	// ESCキーが押されたらループを抜ける
	if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
		break;
	}
}

// ライブラリの終了
Novice::Finalize();
return 0;
}