
#include "Matrix4.h"

const Matrix4 Matrix4::Identity;

Matrix4::Matrix4()
	: m00(1.f), m01(0.f), m02(0.f), m03(0.f),
	  m10(0.f), m11(1.f), m12(0.f), m13(0.f),
	  m20(0.f), m21(0.f), m22(1.f), m23(0.f),
	  m30(0.f), m31(0.f), m32(0.f), m33(1.f)
{
	// set to the identity matrix
}

Matrix4::Matrix4(const Matrix4& rhs)
{
	*this = rhs;
}

Matrix4& Matrix4::operator=(const Matrix4& rhs)
{
	memcpy(m, rhs.m, sizeof(this));
	return *this;
}

Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
	Matrix4 temp;
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			temp.m[row][col] = m[row][0] * rhs.m[0][col] + 
							   m[row][1] * rhs.m[1][col] + 
							   m[row][2] * rhs.m[2][col] +
							   m[row][3] * rhs.m[3][col];
		}
	}

	return temp;
}

Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
	*this = *this * rhs;
	return *this;
}

Vector4 Matrix4::operator*(const Vector4& rhs) const
{
	return Vector4( m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03 * rhs.w,
					m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13 * rhs.w,
					m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23 * rhs.w,
					m30 * rhs.x + m31 * rhs.y + m32 * rhs.z + m33 * rhs.w );
}

Matrix4& Matrix4::CreatePerspectiveMatrix(const float aspect, const float zNear, const float zFar)
{
	// hacky hack thing first
	DBG_ASSERT(zNear - zFar != 0.f);

	m[0][0] = 1.f / aspect;
	m[1][1] = 1.f;
	m[2][2] = (zFar + zNear) / (zNear - zFar);
	m[2][3] = -1.f;
	m[3][2] = (2 * zFar * zNear) / (zNear - zFar);
	m[3][3] = 0.f;

	return *this;
}

float* Matrix4::GetData()
{
	return &m[0][0];
}