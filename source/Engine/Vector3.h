
#pragma once

class Vector3
{
public:
	static const Vector3 Zero;

	Vector3();
	Vector3(const Vector3& rhs);
	Vector3(const float x, const float y, const float z);

	Vector3& operator=(const Vector3& rhs);

	Vector3 operator*(const float scalar) const;
	Vector3& operator*=(const float scalar);
	friend Vector3 operator*(const float scalar, const Vector3& rhs);

	Vector3 operator/(const float divisor) const;
	Vector3& operator/=(const float divisor);

	Vector3 operator+(const Vector3& rhs) const;
	Vector3& operator+=(const Vector3& rhs);
	Vector3 operator-(const Vector3& rhs) const;
	Vector3& operator-=(const Vector3& rhs);
	Vector3 operator-() const;

	float operator*(const Vector3& rhs) const;
	Vector3 operator|(const Vector3& rhs) const;

	float& operator[](const int index);

	float Length() const;
	float LengthSqrd() const;

	void Normalize();
	Vector3 Normal() const;

#pragma warning(push)
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
	union
	{
		float v[3];
		struct
		{
			float x, y, z;
		};
	};
#pragma warning(pop)
};
