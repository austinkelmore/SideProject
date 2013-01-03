
#pragma once

class Matrix3
{
public:
	static const Matrix3 Identity;

	Matrix3();
	Matrix3(const Matrix3& rhs);

	Matrix3& operator=(const Matrix3& rhs);

	Matrix3 operator*(const Matrix3& rhs) const;

	union
	{
		float m[3][3];
		struct
		{
			float m00, m01, m02,
				  m10, m11, m12,
				  m20, m21, m22;
		};
	};
};
