
#include "Matrix.h"

const Matrix3 Matrix3::Identity;

Matrix3::Matrix3()
	: m00(1.f), m01(0.f), m02(0.f),
	  m10(0.f), m11(1.f), m12(0.f),
	  m20(0.f), m21(0.f), m22(1.f)
{
	// set to the identity matrix
}

Matrix3::Matrix3(const Matrix3& rhs)
{
	memcpy(m, rhs.m, sizeof(this));
}

Matrix3& Matrix3::operator=(const Matrix3& rhs)
{
	memcpy(m, rhs.m, sizeof(this));
	return *this;
}

Matrix3 Matrix3::operator*(const Matrix3& rhs) const
{
	Matrix3 temp;
	for (int row = 0; row < 3; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			temp.m[row][col] = m[row][0] * rhs.m[0][col] + 
							   m[row][1] * rhs.m[1][col] + 
							   m[row][2] * rhs.m[2][col];
		}
	}

	return temp;
}
