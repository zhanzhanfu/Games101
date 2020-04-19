#include "Scene.hpp"


void Scene::buildBVH()
{
	printf("Generating Scene BVH......");
	bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

bool Scene::intersect(const Ray &ray, Hit &hit) const
{
	return bvh->intersect(ray, hit);
}

void Scene::sampleLight(Hit &hit, float &pdf) const
{
	float emit_area_sum = 0;
	for (auto object : objects)
	{
		if (object->hasEmit())
		{
			emit_area_sum += object->getArea();
		}
	}
	float p = get_random_float() * emit_area_sum;
	emit_area_sum = 0;
	for (auto object : objects)
	{
		if (object->hasEmit())
		{
			emit_area_sum += object->getArea();
			if (p <= emit_area_sum)
			{
				object->Sample(hit, pdf);
				break;
			}
		}
	}
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
	Hit hit;
	if(!intersect(ray, hit))
		return Vector3f();
	if (hit.m->hasEmit())
		return hit.m->emit;
	Vector3f wo = -ray.direction;
	Vector3f L_dir, L_indir;
	//L_dir
	Hit x;
	float pdf_light;
	sampleLight(x, pdf_light);
	Hit hit_blocked;
	intersect(Ray(hit.p, (x.p - hit.p).normalize()), hit_blocked);
	if (hit_blocked.t + EPSILON > (x.p - hit.p).length())
	{
		Vector3f wi = (x.p - hit.p).normalize();
		float d2 = (x.p - hit.p).length2();
		L_dir = x.m->emit * hit.m->eval(wo, wi, hit.normal) * hit.normal.dot(wi) * x.normal.dot(-wi) / (d2 * pdf_light);
	}

	//L_indir
	if (get_random_float() < RussianRoulette)
	{
		Vector3f wi = hit.m->sample(wo, hit.normal);
		Ray ray_indir(hit.p, wi);
		Hit hit_indir;
		if (intersect(ray_indir, hit_indir) && !hit_indir.m->hasEmit())
			L_indir = castRay(ray_indir, depth) * hit.m->eval(wo, wi, hit.normal) * hit.normal.dot(wi) / (hit.m->pdf(wo, wi, hit.normal) * RussianRoulette);
	}

	return L_dir + L_indir;


}