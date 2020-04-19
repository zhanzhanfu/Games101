#pragma once
#include "Object.hpp"
#include "Material.hpp"
#include "Hit.hpp"

class Triangle : public Object
{
public:
	Vector3f v0, v1, v2; // vertices A, B ,C , counter-clockwise order
	Vector3f e1, e2;     // 2 edges v1-v0, v2-v0;
	Vector3f t0, t1, t2; // texture coords
	Vector3f normal;
	float area;
	Material *m;

	Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material *_m = nullptr)
		: v0(_v0), v1(_v1), v2(_v2), m(_m)
	{
		e1 = v1 - v0;
		e2 = v2 - v0;
		normal = e1.cross(e2).normalize();
		area = e1.cross(e2).length() * 0.5f;
	}

	bool intersect(const Ray &ray, Hit &hit) const override
	{
		Vector3f ro = ray.origin;
		Vector3f rd = ray.direction;
		Vector3f s = ro - v0;
		Vector3f s1 = rd.cross(e2);
		Vector3f s2 = s.cross(e1);
		float k = 1.0f / s1.dot(e1);
		float t = s2.dot(e2) * k;
		float u = s1.dot(s) * k;
		float v = s2.dot(rd) * k;
		if (t > EPSILON && t < hit.t && u > 0 && v > 0 && (1 - u - v) > 0 && u < 1 && v < 1 && (1 - u - v) < 1)
		{
			hit.t = t;
			hit.p = ro + t * rd;
			hit.normal = normal;
			hit.m = m;
			return true;
		}
		return false;
	}

	Bounds3 getBounds() const override { return Union(Bounds3(v0, v1), v2); }
	void Sample(Hit &hit, float &pdf) const override
	{
		float x = std::sqrt(get_random_float()), y = get_random_float();
		hit.p = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
		//float u, v;
		//do
		//{
		//	u = get_random_float();
		//	v = get_random_float();
		//} while ((u + v) > 1.0 - EPSILON);
		//hit.p = v0 * (1.0f - u-v) + v1 * u + v2 * v;
		hit.normal = normal;
		hit.m = m;
		pdf = 1.0f / area;
	}
	float getArea() const override { return area; }

	bool hasEmit() const override { return m->hasEmit(); }

};




