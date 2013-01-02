
#include "Vector.h"

Vector3::Vector3()
	: x(0.f), y(0.f) ,z(0.f)
{
	// do nothing
}

Vector3::Vector3(const Vector3& rhs)
	: x(rhs.x), y(rhs.y), z(rhs.z)
{
	// do nothing
}

Vector3::Vector3(const float x1, const float y1, const float z1)
	: x(x1), y(y1), z(z1)
{
	// do nothing
}

Vector3 Vector3::operator*(const float scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

Vector3 operator*(const float scalar, const Vector3& rhs)
{
	return rhs * scalar;
}

Vector3 Vector3::operator/(const float divisor) const
{
	return Vector3(x / divisor, y / divisor, z / divisor);
}

Vector3 Vector3::operator/=(const float divisor)
{
	x /= divisor;
	y /= divisor;
	z /= divisor;

	return *this;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator+=(const Vector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator-=(const Vector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

// dot product
float Vector3::operator*(const Vector3& rhs) const
{
	return (x * rhs.x + y * rhs.y + z * rhs.z);
}

Vector3 Vector3::operator|(const Vector3& rhs) const
{
	Vector3 temp;

	temp.x = (y * rhs.z) - (rhs.y * z);
	temp.y = (z * rhs.x) - (rhs.z * x);
	temp.z = (x * rhs.y) - (rhs.x * y);

	return temp;
}

float Vector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vector3::LengthSqrd() const
{
	return (x * x + y * y + z * z);
}

void Vector3::Normalize()
{
	float length = Length();

	// ASSERT(length_squared != 0.f);

	float inv_length = 1.f / length;
	
	x *= inv_length;
	y *= inv_length;
	z *= inv_length;
}

Vector3 Vector3::Normal() const
{
	float length = Length();

	// ASSERT(length_squared != 0.f);

	float inv_length = 1.f / length;

	return Vector3(x * inv_length, y * inv_length, z * inv_length);
}
