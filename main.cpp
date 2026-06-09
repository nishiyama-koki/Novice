#include "MyMath.h"
#include "Sphere.h"
#include <Novice.h>
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
}
struct Triangle {
	Vector3 vertices[3];
};
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 screenVertices[3];
	for (uint32_t i = 0; i < 3; ++i) {
		screenVertices[i] = Transform(Transform(triangle.vertices[i], viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), color);
	Novice::DrawLine(int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), color);
	Novice::DrawLine(int(screenVertices[2].x), int(screenVertices[2].y), int(screenVertices[0].x), int(screenVertices[0].y), color);
}
bool isCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 v1 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v2 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 normal = Cross(v1, v2);
	Vector3 normalNormalize = Normalize(normal);

	float dot = Dot(normalNormalize, segment.diff);
	if (dot == 0.0f) {
		return false;
	}

	float distance = Dot(triangle.vertices[0], normalNormalize);
	float t = (distance - Dot(segment.origin, normalNormalize)) / dot;
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Vector3 p = Add(segment.origin, Multiply(segment.diff, t));
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);
	Vector3 v0p = Subtract(p, triangle.vertices[0]);
	Vector3 v1p = Subtract(p, triangle.vertices[1]);
	Vector3 v2p = Subtract(p, triangle.vertices[2]);
	Vector3 cross01 = Cross(v01, v0p);
	Vector3 cross12 = Cross(v12, v1p);
	Vector3 cross20 = Cross(v20, v2p);
	if (Dot(cross01, normalNormalize) >= 0.0f && Dot(cross12, normalNormalize) >= 0.0f && Dot(cross20, normalNormalize) >= 0.0f) {
		return true;
	} else {
		return false;
	}

} 
//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

// ライブラリの初期化
Novice::Initialize(kWindowTitle, 1280, 720);

// キー入力結果を受け取る箱
char keys[256] = {0};
char preKeys[256] = {0};

Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
Triangle triangle{
    {{-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}
};
Segment segment{
    {0.0f, 1.0f,  0.0f},
    {0.0f, -2.0f, 0.0f}
};

uint32_t segmentColor = WHITE;

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

	if (ImGui::CollapsingHeader("Segment")) {
		ImGui::DragFloat3("Segment Origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment Diff", &segment.diff.x, 0.01f);
	}

	if (ImGui::CollapsingHeader("Triangle")) {
		ImGui::DragFloat3("Vertex0", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("Vertex1", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("Vertex2", &triangle.vertices[2].x, 0.01f);
	}

	ImGui::End();

	if (isCollision(triangle, segment)) {
		segmentColor = RED;
	} else {
		segmentColor = WHITE;
	}

	Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraTranslate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

	Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

	///
	/// ↑更新処理ここまで
	///

	///
	/// ↓描画処理ここから
	///

	DrawGrid(viewProjectionMatrix, viewportMatrix);
	Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), segmentColor);
	DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, WHITE);

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
