
#include "Vector2.h"
#include "BasicMacros.h"

const Vector2 Vector2::Zero;

Vector2::Vector2()
	: x(0.f), y(0.f)
{
	// do nothing
}

Vector2::Vector2(const Vector2& rhs)
	: x(rhs.x), y(rhs.y)
{
	// do nothing
}

Vector2::Vector2(const float x1, const float y1)
	: x(x1), y(y1)
{
	// do nothing
}

Vector2& Vector2::operator=(const Vector2& rhs)
{
	memcpy(this, &rhs, sizeof(this));
	return *this;
}

Vector2 Vector2::operator*(const float scalar) const
{
	return Vector2(x * scalar, y * scalar);
}

Vector2& Vector2::operator*=(const float scalar)
{
	*this = *this * scalar;
	return *this;
}

Vector2 operator*(const float scalar, const Vector2& rhs)
{
	return rhs * scalar;
}

Vector2 Vector2::operator/(const float divisor) const
{
	DBG_ASSERT(divisor != 0.f);
	return Vector2(x / divisor, y / divisor);
}

Vector2& Vector2::operator/=(const float divisor)
{
	*this = *this / divisor;
	return *this;
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
	return Vector2(x + rhs.x, y + rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

// dot product
float Vector2::operator*(const Vector2& rhs) const
{
	return (x * rhs.x + y * rhs.y);
}

// cross product
float Vector2::operator|(const Vector2& rhs) const
{
	return (x * rhs.y - rhs.x * y);
}

float& Vector2::operator[](const int index)
{
	DBG_ASSERT(0 <= index && index < 3);
	return v[index];
}

float Vector2::Length() const
{
	return sqrtf(x * x + y * y);
}

float Vector2::LengthSqrd() const
{
	return (x * x + y * y);
}

void Vector2::Normalize()
{
	float length = Length();

	DBG_ASSERT_MSG(length != 0.f, "Vector 2 Normalizing with 0 length.");

	float inv_length = 1.f / length;

	x *= inv_length;
	y *= inv_length;
}

Vector2 Vector2::Normal() const
{
	float length = Length();

	DBG_ASSERT_MSG(length != 0.f, "Vector 2 Normalizing with 0 length.");

	float inv_length = 1.f / length;

	return Vector2(x * inv_length, y * inv_length);
}
