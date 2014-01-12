
#include "Vector4.h"
#include "BasicMacros.h"

const Vector4 Vector4::Zero;

Vector4::Vector4()
	: x(0.f), y(0.f), z(0.f), w(0.f)
{
	// do nothing
}

Vector4::Vector4(const Vector4& rhs)
	: x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
	// do nothing
}

Vector4::Vector4(const float x1, const float y1, const float z1, const float w1)
	: x(x1), y(y1), z(z1), w(w1)
{
	// do nothing
}

Vector4& Vector4::operator=(const Vector4& rhs)
{
	memcpy(this, &rhs, sizeof(this));
	return *this;
}

Vector4 Vector4::operator*(const float scalar) const
{
	return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4& Vector4::operator*=(const float scalar)
{
	*this = *this * scalar;
	return *this;
}

Vector4 operator*(const float scalar, const Vector4& rhs)
{
	return rhs * scalar;
}

Vector4 Vector4::operator/(const float divisor) const
{
	DBG_ASSERT(divisor != 0.f);
	return Vector4(x / divisor, y / divisor, z / divisor, w / divisor);
}

Vector4& Vector4::operator/=(const float divisor)
{
	*this = *this / divisor;
	return *this;
}

Vector4 Vector4::operator+(const Vector4& rhs) const
{
	return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Vector4& Vector4::operator+=(const Vector4& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector4 Vector4::operator-(const Vector4& rhs) const
{
	return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Vector4& Vector4::operator-=(const Vector4& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

// dot product
float Vector4::operator*(const Vector4& rhs) const
{
	return (x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}

// cross product
Vector4 Vector4::operator|(const Vector4& rhs) const
{
	return Vector4( (y * rhs.z) - (rhs.y * z),
					(z * rhs.x) - (rhs.z * x),
					(x * rhs.y) - (rhs.x * y),
					0.f );
}

float& Vector4::operator[](const int index)
{
	DBG_ASSERT(0 <= index && index < 4);
	return v[index];
}

float Vector4::Length() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

float Vector4::LengthSqrd() const
{
	return (x * x + y * y + z * z + w * w);
}

void Vector4::Normalize()
{
	float length = Length();

	DBG_ASSERT_MSG(length != 0.f, "Vector 3 Normalizing with 0 length.");

	float inv_length = 1.f / length;

	x *= inv_length;
	y *= inv_length;
	z *= inv_length;
	w *= inv_length;
}

Vector4 Vector4::Normal() const
{
	float length = Length();

	DBG_ASSERT_MSG(length != 0.f, "Vector 3 Normalizing with 0 length.");

	float inv_length = 1.f / length;

	return Vector4(x * inv_length, y * inv_length, z * inv_length, w * inv_length);
}
