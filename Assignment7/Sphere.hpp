#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include "Object.hpp"
#include "Material.hpp"

class Sphere : public Object
{
public:
	Vector3f center;
	float radius, radius2;
	Material *m;
	float area;
	Sphere(const Vector3f &c, const float &r, Material *mt = new Material()) : center(c), radius(r), radius2(r *r), m(mt), area(4 * M_PI * r * r) {}
	bool intersect(const Ray &ray, Hit &hit) const override
	{
		Vector3f ro = ray.origin;
		Vector3f rd = ray.direction;
		Vector3f L = ro - center;
		float a = rd.dot(rd);
		float b = 2 * rd.dot(L);
		float c = L.dot(L) - radius2;
		float t0, t1;
		if (!solveQuadratic(a, b, c, t0, t1)) return false;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		if (t0 < hit.t)
		{
			hit.t = t0;
			hit.p = ro + t0 * rd;
			hit.normal = (hit.p - center) / radius;
			hit.m = m;
			return true;
		}
		return false;
	}

	Bounds3 getBounds() const override
	{
		return Bounds3(Vector3f(center.x - radius, center.y - radius, center.z - radius),
			Vector3f(center.x + radius, center.y + radius, center.z + radius));
	}
	void Sample(Hit &hit, float &pdf) const override
	{
		float theta = 2.0 * M_PI * get_random_float(), phi = M_PI * get_random_float();
		Vector3f dir(std::cos(phi), std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta));
		hit.p = center + radius * dir;
		hit.normal = dir;
		hit.m = m;
		pdf = 1.0f / area;
	}
	float getArea() const override { return area; }
	bool hasEmit() const override { return m->hasEmit(); }
};




#endif //RAYTRACING_SPHERE_H
