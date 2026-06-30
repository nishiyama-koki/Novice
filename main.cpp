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
}
// AABBの描画
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
bool IsCollision(const AABB& aabb, const Segment& segment) {
	// 0除算を避けるための小さな値
	const float kEpsilon = 0.00001f;

	// 各軸のtのmin/maxを入れる変数（std::min/maxを隠蔽しない名前に変更）
	Vector3 tMinXYZ{};
	Vector3 tMaxXYZ{};

	// X軸の計算（0除算対策付き）
	if (std::abs(segment.diff.x) < kEpsilon) {
		if (segment.origin.x < aabb.min.x || segment.origin.x > aabb.max.x) {
			return false;
		}
		tMinXYZ.x = -std::numeric_limits<float>::infinity();
		tMaxXYZ.x = std::numeric_limits<float>::infinity();
	} else {
		tMinXYZ.x = (aabb.min.x - segment.origin.x) / segment.diff.x;
		tMaxXYZ.x = (aabb.max.x - segment.origin.x) / segment.diff.x;
	}

	// Y軸の計算（0除算対策付き）
	if (std::abs(segment.diff.y) < kEpsilon) {
		if (segment.origin.y < aabb.min.y || segment.origin.y > aabb.max.y) {
			return false;
		}
		tMinXYZ.y = -std::numeric_limits<float>::infinity();
		tMaxXYZ.y = std::numeric_limits<float>::infinity();
	} else {
		tMinXYZ.y = (aabb.min.y - segment.origin.y) / segment.diff.y;
		tMaxXYZ.y = (aabb.max.y - segment.origin.y) / segment.diff.y;
	}

	// Z軸の計算（0除算対策付き）
	if (std::abs(segment.diff.z) < kEpsilon) {
		if (segment.origin.z < aabb.min.z || segment.origin.z > aabb.max.z) {
			return false;
		}
		tMinXYZ.z = -std::numeric_limits<float>::infinity();
		tMaxXYZ.z = std::numeric_limits<float>::infinity();
	} else {
		tMinXYZ.z = (aabb.min.z - segment.origin.z) / segment.diff.z;
		tMaxXYZ.z = (aabb.max.z - segment.origin.z) / segment.diff.z;
	}

	// near far を求める (括弧で囲むことで Windowsマクロ の展開を強制防止)
	Vector3 near_ = {(std::min)(tMinXYZ.x, tMaxXYZ.x), (std::min)(tMinXYZ.y, tMaxXYZ.y), (std::min)(tMinXYZ.z, tMaxXYZ.z)};
	Vector3 far_ = {(std::max)(tMinXYZ.x, tMaxXYZ.x), (std::max)(tMinXYZ.y, tMaxXYZ.y), (std::max)(tMinXYZ.z, tMaxXYZ.z)};

	// AABBとの衝突点(貫通点)の t が小さい方
	float tmin = (std::max)((std::max)(near_.x, near_.y), near_.z);

	// AABBとの衝突点(貫通点)の t が大きい方
	float tmax = (std::min)((std::min)(far_.x, far_.y), far_.z);

	// 線分としての範囲条件（tmin <= tmax かつ、線分の範囲 0.0f 〜 1.0f と重複しているか）
	if (tmin <= tmax && tmin <= 1.0f && tmax >= 0.0f) {
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
	Segment segment{
	    {0.0f, 2.0f,  0.0f},
        {0.0f, -4.0f, 0.0f}
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

		if (ImGui::CollapsingHeader("AABB1")) {
			ImGui::DragFloat3("AABB1.min", &aabb1.min.x, 0.01f);
			ImGui::DragFloat3("AABB1.max", &aabb1.max.x, 0.01f);
		}

		if (ImGui::CollapsingHeader("Segment")) {
			ImGui::DragFloat3("Segment Origin", &segment.origin.x, 0.01f);
			ImGui::DragFloat3("Segment Diff", &segment.diff.x, 0.01f);
		}

		ImGui::End();

		if (IsCollision(aabb1, segment)) {
			aabb1Color = RED;
			segmentColor = RED;
		} else {
			aabb1Color = WHITE;
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
		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, aabb1Color);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), segmentColor);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	} // ライブラリの終了
	Novice::Finalize();
	return 0;
}