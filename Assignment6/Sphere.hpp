#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include "Object.hpp"
#include "Vector.hpp"
#include "Bounds3.hpp"
#include "Material.hpp"

class Sphere : public Object
{
public:
	Vector3f center;
	float radius, radius2;
	Material *m;

	Sphere(const Vector3f &c, const float &r) : center(c), radius(r), radius2(r *r), m(new Material()) {}
	bool intersect(const Ray &ray) const override
	{
		// analytic solution
		Vector3f L = ray.origin - center;
		float a = dotProduct(ray.direction, ray.direction);
		float b = 2 * dotProduct(ray.direction, L);
		float c = dotProduct(L, L) - radius2;
		float t0, t1;
		if (!solveQuadratic(a, b, c, t0, t1)) return false;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		return true;
	}
	bool intersect(const Ray &ray, float &tnear, uint32_t &index) const override
	{
		// analytic solution
		Vector3f L = ray.origin - center;
		float a = dotProduct(ray.direction, ray.direction);
		float b = 2 * dotProduct(ray.direction, L);
		float c = dotProduct(L, L) - radius2;
		float t0, t1;
		if (!solveQuadratic(a, b, c, t0, t1)) return false;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		tnear = t0;

		return true;
	}
	Intersection getIntersection(const Ray &ray) override
	{
		Intersection result;
		result.happened = false;
		Vector3f L = ray.origin - center;
		float a = dotProduct(ray.direction, ray.direction);
		float b = 2 * dotProduct(ray.direction, L);
		float c = dotProduct(L, L) - radius2;
		float t0, t1;
		if (!solveQuadratic(a, b, c, t0, t1)) return result;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return result;
		result.happened = true;

		result.coords = Vector3f(ray.origin + ray.direction * t0);
		result.normal = normalize(Vector3f(result.coords - center));
		result.m = this->m;
		result.obj = this;
		result.distance = t0;
		return result;

	}
	void getSurfaceProperties(const Vector3f &P, const Vector3f &I, const uint32_t &index, const Vector2f &uv, Vector3f &N, Vector2f &st) const override
	{
		N = normalize(P - center);
	}

	Vector3f evalDiffuseColor(const Vector2f &st)const override
	{
		return m->getColor();
	}
	Bounds3 getBounds() const override
	{
		return Bounds3(Vector3f(center.x - radius, center.y - radius, center.z - radius),
			Vector3f(center.x + radius, center.y + radius, center.z + radius));
	}
};


#endif //RAYTRACING_SPHERE_H
