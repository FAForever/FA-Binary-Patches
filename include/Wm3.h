#pragma once

template<class R>
struct Vector2
{
	R x, y;
};
using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;

template<class R>
class Vector3
{
public:
	using value_t = R;

	R x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(R x_, R y_, R z_) : x(x_), y(y_), z(z_) {}

	Vector3(const Vector3<R> &other) : x(other.x), y(other.y), z(other.z) {}

	Vector3<R> &operator=(const Vector3<R> &other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}

	Vector3<R> operator+(const Vector3<R> &other) const
	{
		return Vector3<R>(x + other.x, y + other.y, z + other.z);
	}

	Vector3<R> operator-(const Vector3<R> &other) const
	{
		return Vector3<R>(x - other.x, y - other.y, z - other.z);
	}

	Vector3<R> operator*(R scalar) const
	{
		return Vector3<R>(x * scalar, y * scalar, z * scalar);
	}

	friend Vector3<R> operator*(R scalar, const Vector3<R> &vec)
	{
		return vec * scalar;
	}

	R dot(const Vector3<R> &other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3<R> cross(const Vector3<R> &other) const
	{
		return Vector3<R>(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3<float> normalized() const
	{
		float len = length();
		if (len > 0)
			return {x / len, y / len, z / len};

		return *this;
	}

	Vector3<R> &operator+=(const Vector3<R> &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3<R> &operator-=(const Vector3<R> &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3<R> &operator*=(R scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
};
using Vector3f = Vector3<float>;

template<class R>
struct Vector4
{ // 0x10 bytes
	R x, y, z, w;
};
using Vector4f = Vector4<float>;

template<class R>
struct Quaternion
{
    R x, y, z, w;
};
using Quaternionf = Quaternion<float>;

template<class R>
struct Rect2
{
	R x0, z0, x1, z1;
};
using Rect2i = Rect2<int>;

template<class R>
struct Circle2
{
	Vector2<R> Center;
	R Radius;

	Circle2(R x, R y, R r):
		Center{x, y},
		Radius{r}
	{}
};
using Circle2f = Circle2<float>;

template<class R>
struct Plane3
{
	Vector3<R> Normal;
	R Constant;
};
using Plane3f = Plane3<float>;

template<class R>
struct AxisAlignedBox3
{
	Vector3<R> Min;
	Vector3<R> Max;
};
using AxisAlignedBox3f = AxisAlignedBox3<float>;
