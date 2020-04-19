#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"

struct Hit
{
	Hit()
	{
		p = Vector3f();
		normal = Vector3f();
		t = std::numeric_limits<float>::max();
		m = nullptr;
	}
	Vector3f p;
	Vector3f normal;
	float t;
	Material *m;
};
#endif //RAYTRACING_INTERSECTION_H
