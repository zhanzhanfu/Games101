#pragma once
#ifndef RAYTRACING_VECTOR_H
#define RAYTRACING_VECTOR_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cassert>
class Vector3f
{
public:
	float x, y, z;
	Vector3f() : x(0), y(0), z(0) {}
	Vector3f(float xx) : x(xx), y(xx), z(xx) {}
	Vector3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	float length2() const { return x * x + y * y + z * z; }
	float length() const { return std::sqrt(length2()); }
	Vector3f operator * (float r) const { return Vector3f(x * r, y * r, z * r); }
	Vector3f operator / (float r) const { return Vector3f(x / r, y / r, z / r); }
	Vector3f operator * (const Vector3f &v) const { return Vector3f(x * v.x, y * v.y, z * v.z); }
	Vector3f operator - (const Vector3f &v) const { return Vector3f(x - v.x, y - v.y, z - v.z); }
	Vector3f operator + (const Vector3f &v) const { return Vector3f(x + v.x, y + v.y, z + v.z); }
	Vector3f operator - () const { return Vector3f(-x, -y, -z); }
	Vector3f &operator += (const Vector3f &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vector3f &operator /= (float r) { x /= r, y /= r, z /= r; return *this; }
	double operator[](int index) const { return (&x)[index]; }
	//double &operator[](int index);
	Vector3f normalize()
	{
		float r = length();
		assert(r > 0);
		float inv = 1.0 / r;
		return Vector3f(x * inv, y * inv, z * inv);
	}
	float dot(const Vector3f &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	Vector3f cross(const Vector3f &v) const
	{
		return Vector3f(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}
	friend Vector3f operator * (const float &r, const Vector3f &v)
	{
		return Vector3f(v.x * r, v.y * r, v.z * r);
	}
	friend std::ostream &operator << (std::ostream &os, const Vector3f &v)
	{
		return os << v.x << ", " << v.y << ", " << v.z;
	}



	static Vector3f Min(const Vector3f &p1, const Vector3f &p2)
	{
		return Vector3f(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
	}

	static Vector3f Max(const Vector3f &p1, const Vector3f &p2)
	{
		return Vector3f(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
	}
};

class Vector2f
{
public:
	float x, y;
	Vector2f() : x(0), y(0) {}
	Vector2f(float xx) : x(xx), y(xx) {}
	Vector2f(float xx, float yy) : x(xx), y(yy) {}
	Vector2f operator * (const float &r) const { return Vector2f(x * r, y * r); }
	Vector2f operator + (const Vector2f &v) const { return Vector2f(x + v.x, y + v.y); }
};

inline Vector3f lerp(const Vector3f &a, const Vector3f &b, const float &t)
{
	return a * (1 - t) + b * t;
}

#endif //RAYTRACING_VECTOR_H
