
#pragma once

#include "Vector4.h"

class Matrix4
{
public:
	static const Matrix4 Identity;

	Matrix4();
	Matrix4(const Matrix4& rhs);

	Matrix4& operator=(const Matrix4& rhs);

	Matrix4 operator*(const Matrix4& rhs) const;
	Matrix4& operator*=(const Matrix4& rhs);

	// vector operators
	Vector4 operator*(const Vector4& rhs) const;

	Matrix4& CreatePerspectiveMatrix(const float aspect, const float zNear, const float zFar);
	float* GetData();

#pragma warning(push)
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
	union
	{
		float m[4][4];
		struct
		{
			float m00, m01, m02, m03,
				  m10, m11, m12, m13,
				  m20, m21, m22, m23,
				  m30, m31, m32, m33;
		};
	};
#pragma warning(pop)
};
