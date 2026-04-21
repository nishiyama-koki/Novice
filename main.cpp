#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "GC2A_ニシヤマ_コウキ";

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

#pragma region Vector3関数

// ベクトルの加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

// ベクトルの減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

// ベクトルのスカラー倍
Vector3 Multiply(const Vector3& v, float scalar) {
	Vector3 result{};
	result.x = v.x * scalar;
	result.y = v.y * scalar;
	result.z = v.z * scalar;
	return result;
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// 長さ(ノルム)
float Length(const Vector3& v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

// 正規化
Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	if (length == 0.0f) {
		return Vector3{0.0f, 0.0f, 0.0f};
	}
	return Multiply(v, 1.0f / length);
}

static const int kColumnWidth = 60;
static const int kRowHeight = 20;
void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", v.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", v.y);
	Novice::ScreenPrintf(x + 2 * kColumnWidth, y, "%.02f", v.z);
	;
	Novice::ScreenPrintf(x + 3 * kColumnWidth, y, "%s", label);
	;
}

#pragma endregion

#pragma region Matrix4x4関数

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentityMatrix4x4() {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

//逆行列
//  逆行列（ガウス・ジョルダン法）
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = m;
	Matrix4x4 identity = MakeIdentityMatrix4x4();

	for (int i = 0; i < 4; ++i) {
		// ピボットが0の場合は簡易的に単位行列を返す
		if (result.m[i][i] == 0.0f) {
			return MakeIdentityMatrix4x4();
		}

		float pivot = result.m[i][i];
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = static_cast<float>(result.m[i][j] / pivot);
			identity.m[i][j] = static_cast<float>(identity.m[i][j] / pivot);
		}

		for (int k = 0; k < 4; ++k) {
			if (k != i) {
				float factor = result.m[k][i];
				for (int j = 0; j < 4; ++j) {
					result.m[k][j] -= factor * result.m[i][j];
					identity.m[k][j] -= factor * identity.m[i][j];
				}
			}
		}
	}
	return identity;
}


//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}



static const int kMatrixRowHeight = 20;
static const int kMatrixColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& m, const char* label) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			Novice::ScreenPrintf(x + j * kMatrixColumnWidth, y + i * kMatrixRowHeight, "%6.02f", m.m[i][j]);
		}
	}
	Novice::ScreenPrintf(x + 4 * kMatrixColumnWidth, y + 2 * kMatrixRowHeight, "%s", label);
}

#pragma endregion






// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

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

		/*Vector3 v1{1.0f, 3.0f, -5.0f};
		Vector3 v2{4.0f, -1.0f, 2.0f};

		float k = 4.0f;

		Vector3 resultAdd = Add(v1, v2);
		Vector3 resultSubtract = Subtract(v1, v2);
		Vector3 resultMultiply = Multiply(v1, k);
		float resultDot = Dot(v1, v2);
		float resultLength = Length(v1);
		Vector3 resultNormalize = Normalize(v2);*/

		Matrix4x4 m1={3.2f,0.7f,9.6f,4.4f,
			5.5f, 1.3f, 7.8f, 2.1f,
			6.9f, 8.0f, 2.6f, 1.0f, 
			0.5f, 7.2f, 5.1f, 3.3f};

		Matrix4x4 m2 = {4.1f, 6.5f, 3.3f, 2.2f,
			8.8f, 0.6f, 9.9f, 7.7f, 
			1.1f, 5.5f, 6.6f, 0.0f, 
			3.3f, 9.9f, 8.8f, 2.2f};

		Matrix4x4 resultAdd = Add(m1, m2);
		Matrix4x4 resultSubtract = Subtract(m1, m2);	
		Matrix4x4 resultMultiply = Multiply(m1, m2);
		Matrix4x4 resultInverse = Inverse(m1);
		Matrix4x4 resultInverse2 = Inverse(m2);
		Matrix4x4 resultTranspose = Transpose(m1);
		Matrix4x4 resultTranspose2 = Transpose(m2);
		Matrix4x4 resultIdentity = MakeIdentityMatrix4x4();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		/*VectorScreenPrintf(0, 0, resultAdd, ":Add");
		VectorScreenPrintf(0, kRowHeight, resultSubtract, ":Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, resultMultiply, ":Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f:Dot", resultDot);
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f:Length", resultLength);
		VectorScreenPrintf(0, kRowHeight * 5, resultNormalize, ":Normalize");*/

		MatrixScreenPrintf(0, 0, resultAdd, "Add");
		MatrixScreenPrintf(0, kMatrixRowHeight * 5, resultSubtract, "Subtract");
		MatrixScreenPrintf(0, kMatrixRowHeight * 5*2, resultMultiply, "Multiply");
		MatrixScreenPrintf(0, kMatrixRowHeight * 5*3, resultInverse, "Inverse");
		MatrixScreenPrintf(0, kMatrixRowHeight * 5*4, resultInverse2, "Inverse2");
		MatrixScreenPrintf(kMatrixColumnWidth*5,0,resultTranspose, "Transpose");
		MatrixScreenPrintf(kMatrixColumnWidth*5, kMatrixRowHeight * 5, resultTranspose2, "Transpose2");
		MatrixScreenPrintf(kMatrixColumnWidth*5, kMatrixRowHeight * 5*2, resultIdentity, "Identity");

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
