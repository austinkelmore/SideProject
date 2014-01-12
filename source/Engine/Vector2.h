
#pragma once

class Vector2
{
public:
	static const Vector2 Zero;

	Vector2();
	Vector2(const Vector2& rhs);
	Vector2(const float x, const float y);

	Vector2& operator=(const Vector2& rhs);

	Vector2 operator*(const float scalar) const;
	Vector2& operator*=(const float scalar);
	friend Vector2 operator*(const float scalar, const Vector2& rhs);

	Vector2 operator/(const float divisor) const;
	Vector2& operator/=(const float divisor);

	Vector2 operator+(const Vector2& rhs) const;
	Vector2& operator+=(const Vector2& rhs);
	Vector2 operator-(const Vector2& rhs) const;
	Vector2& operator-=(const Vector2& rhs);
	Vector2 operator-() const;

	// dot product
	float operator*(const Vector2& rhs) const;
	// cross product
	float operator|(const Vector2& rhs) const;

	float& operator[](const int index);

	float Length() const;
	float LengthSqrd() const;

	void Normalize();
	Vector2 Normal() const;

#pragma warning(push)
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
	union
	{
		float v[2];
		struct
		{
			float x, y;
		};
	};
#pragma warning(pop)
};
