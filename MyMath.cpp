#include "MyMath.h"
#include <Novice.h>
#include <cmath>

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

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

#pragma endregion

#pragma region Matrix4x4関数

// 行列の加法
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

// 逆行列（ガウス・ジョルダン法）
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

// 転置行列
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

// 座標変換
Vector3 Transform(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{};
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
	result.x = (v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w;
	result.y = (v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w;
	result.z = (v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w;

	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = MakeIdentityMatrix4x4();
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = MakeIdentityMatrix4x4();
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}

#pragma region 回転行列
// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = MakeIdentityMatrix4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[1][1] = cosA;
	result.m[1][2] = sinA;
	result.m[2][1] = -sinA;
	result.m[2][2] = cosA;
	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = MakeIdentityMatrix4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[0][0] = cosA;
	result.m[0][2] = -sinA;
	result.m[2][0] = sinA;
	result.m[2][2] = cosA;
	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = MakeIdentityMatrix4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[0][0] = cosA;
	result.m[0][1] = sinA;
	result.m[1][0] = -sinA;
	result.m[1][1] = cosA;
	return result;
}
#pragma endregion

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	return Multiply(Multiply(scaleMatrix, Multiply(Multiply(rotateXMatrix, rotateYMatrix), rotateZMatrix)), translateMatrix);
}

#pragma region レンダリングパイプライン
Matrix4x4 MakePerspectiveFovMatrix(float fovRadians, float aspect, float nearClip, float farClip) {
	Matrix4x4 result{};
	float top = nearClip * tanf(fovRadians / 2.0f);
	float bottom = -top;
	float right = top * aspect;
	float left = -right;
	result.m[0][0] = (2.0f * nearClip) / (right - left);
	result.m[1][1] = (2.0f * nearClip) / (top - bottom);
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = -(farClip * nearClip) / (farClip - nearClip);
	result.m[3][3] = 0.0f;
	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveMatrix(float left, float right, float top, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};
	result.m[0][0] = (2.0f * nearClip) / (right - left);
	result.m[1][1] = (2.0f * nearClip) / (top - bottom);
	result.m[2][2] = -(farClip + nearClip) / (farClip - nearClip);
	result.m[2][3] = -(2.0f * farClip * nearClip) / (farClip - nearClip);
	result.m[3][2] = -1.0f;
	return result;
}

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;
	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = MakeIdentityMatrix4x4();
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}
#pragma endregion

// 正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	float v2LengthSq = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
	if (v2LengthSq == 0.0f) {
		return result;
	}
	float t = Dot(v1, v2) / v2LengthSq;
	result = Multiply(v2, t);
	return result;
}

// 線分上の最近傍点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result{};
	Vector3 v = Subtract(point, segment.origin);
	float diffLengthSq = segment.diff.x * segment.diff.x + segment.diff.y * segment.diff.y + segment.diff.z * segment.diff.z;
	if (diffLengthSq == 0.0f) {
		return segment.origin;
	}
	float t = Dot(v, segment.diff) / diffLengthSq;
	// クランプ処理
	if (t < 0.0f) {
		t = 0.0f;
	}
	if (t > 1.0f) {
		t = 1.0f;
	}

	result = Add(segment.origin, Multiply(segment.diff, t));
	return result;
}