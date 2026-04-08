#pragma once
#include <cmath>

class Vector3f
{
public:
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

	Vector3f(const Vector3f &other) : x(other.x), y(other.y), z(other.z) {}

	Vector3f &operator=(const Vector3f &other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}

	Vector3f operator+(const Vector3f &other) const
	{
		return Vector3f(x + other.x, y + other.y, z + other.z);
	}

	Vector3f operator-(const Vector3f &other) const
	{
		return Vector3f(x - other.x, y - other.y, z - other.z);
	}

	Vector3f operator*(float scalar) const
	{
		return Vector3f(x * scalar, y * scalar, z * scalar);
	}

	friend Vector3f operator*(float scalar, const Vector3f &vec)
	{
		return vec * scalar;
	}

	float dot(const Vector3f &other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3f cross(const Vector3f &other) const
	{
		return Vector3f(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3f normalized() const
	{
		float len = length();
		if (len > 0)
			return {x / len, y / len, z / len};

		return *this;
	}

	Vector3f &operator+=(const Vector3f &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3f &operator-=(const Vector3f &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3f &operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
};