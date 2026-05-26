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
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), RED);
	}
	// 左から右への線を順々にひいていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + zIndex * kGridEvery;
		Vector3 start = Transform({-kGridHalfWidth, 0.0f, z}, viewPlojectionMatrix4x4);
		Vector3 end = Transform({kGridHalfWidth, 0.0f, z}, viewPlojectionMatrix4x4);
		start = Transform(start, viewPortMatrix4x4);
		end = Transform(end, viewPortMatrix4x4);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), RED);
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
	Segment segment{
	    {-2.0f, -1.0f, 0.0f},
        {3.0f,  2.0f,  2.0f}
    };

	Vector3 point{-1.0f, 0.6f, 0.6f};

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

		if (ImGui::CollapsingHeader("Segment & Point")) {
			ImGui::DragFloat3("Segment Origin", &segment.origin.x, 0.01f);
			ImGui::DragFloat3("Segment Diff", &segment.diff.x, 0.01f);
			ImGui::DragFloat3("Point", &point.x, 0.01f);
		}

		Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
		Vector3 closestPoint = ClosestPoint(point, segment);
		Sphere pointSphere{point, 0.01f};
		Sphere closestPointSphere{closestPoint, 0.01f};

		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::End();
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

		// グリッドの描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 線分の描画
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);

		// 点と最近傍点の描画
		DrawSphere(pointSphere, viewProjectionMatrix, viewportMatrix,0xFF0000FF);
		DrawSphere(closestPointSphere, viewProjectionMatrix, viewportMatrix,0x000000FF);

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