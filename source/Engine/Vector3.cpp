
#include "Vector3.h"
#include "BasicMacros.h"

const Vector3 Vector3::Zero;

Vector3::Vector3()
	: x(0.f), y(0.f), z(0.f)
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

Vector3& Vector3::operator=(const Vector3& rhs)
{
	memcpy(this, &rhs, sizeof(this));
	return *this;
}

Vector3 Vector3::operator*(const float scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3& Vector3::operator*=(const float scalar)
{
	*this = *this * scalar;
	return *this;
}

Vector3 operator*(const float scalar, const Vector3& rhs)
{
	return rhs * scalar;
}

Vector3 Vector3::operator/(const float divisor) const
{
	DBG_ASSERT(divisor != 0.f);
	return Vector3(x / divisor, y / divisor, z / divisor);
}

Vector3& Vector3::operator/=(const float divisor)
{
	*this = *this / divisor;
	return *this;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
	*this = *this - rhs;
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

// cross product
Vector3 Vector3::operator|(const Vector3& rhs) const
{
	return Vector3( (y * rhs.z) - (rhs.y * z),
					(z * rhs.x) - (rhs.z * x),
					(x * rhs.y) - (rhs.x * y) );
}

float& Vector3::operator[](const int index)
{
	DBG_ASSERT(0 <= index && index < 3);
	return v[index];
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

	DBG_ASSERT_MSG(length != 0.f, "Vector 3 Normalizing with 0 length.");

	float inv_length = 1.f / length;
	
	x *= inv_length;
	y *= inv_length;
	z *= inv_length;
}

Vector3 Vector3::Normal() const
{
	float length = Length();

	DBG_ASSERT_MSG(length != 0.f, "Vector 3 Normalizing with 0 length.");

	float inv_length = 1.f / length;

	return Vector3(x * inv_length, y * inv_length, z * inv_length);
}
