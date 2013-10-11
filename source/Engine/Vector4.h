
#pragma once

class Vector4
{
public:
	static const Vector4 Zero;

	Vector4();
	Vector4(const Vector4& rhs);
	Vector4(const float x, const float y, const float z, const float w);

	Vector4& operator=(const Vector4& rhs);

	Vector4 operator*(const float scalar) const;
	Vector4& operator*=(const float scalar);
	friend Vector4 operator*(const float scalar, const Vector4& rhs);

	Vector4 operator/(const float divisor) const;
	Vector4& operator/=(const float divisor);

	Vector4 operator+(const Vector4& rhs) const;
	Vector4& operator+=(const Vector4& rhs);
	Vector4 operator-(const Vector4& rhs) const;
	Vector4& operator-=(const Vector4& rhs);
	Vector4 operator-() const;

	float operator*(const Vector4& rhs) const;
	Vector4 operator|(const Vector4& rhs) const;

	float Length() const;
	float LengthSqrd() const;

	void Normalize();
	Vector4 Normal() const;

	float x, y, z, w;
};
