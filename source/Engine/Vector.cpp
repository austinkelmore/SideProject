
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

Vector3 Vector3::operator*(const float mult) const
{
	return Vector3(x * mult, y * mult, z * mult);
}

Vector3 Vector3::operator*=(const float mult)
{
	x *= mult;
	y *= mult;
	z *= mult;

	return *this;
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
	float length_squared = LengthSqrd();

	// ASSERT(length_squared != 0.f);
	
	x /= length_squared;
	y /= length_squared;
	z /= length_squared;
}

Vector3 Vector3::Normal() const
{
	float length_squared = LengthSqrd();

	// ASSERT(length_squared != 0.f);

	return Vector3(x / length_squared, y / length_squared, z / length_squared);
}
