#ifndef RAYTRACING_BVH_H
#define RAYTRACING_BVH_H

#include <atomic>
#include <vector>
#include <memory>
#include <ctime>
#include "Object.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"
#include "Hit.hpp"
#include "Vector.hpp"

struct BVHBuildNode
{
	Bounds3 bounds;
	BVHBuildNode *left;
	BVHBuildNode *right;
	Object *object;
	float area;

	int splitAxis = 0, firstPrimOffset = 0, nPrimitives = 0;
	BVHBuildNode() :bounds(Bounds3()), left(nullptr), right(nullptr), object(nullptr), area(0) {}
};

class BVHAccel
{
public:
	enum class SplitMethod { NAIVE, SAH };
	// BVHAccel Public Methods
	BVHAccel(std::vector<Object *> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE);
	Bounds3 WorldBound() const;
	~BVHAccel();

	bool intersect(const Ray &ray, Hit &hit) const;
	bool intersectNode(BVHBuildNode *node, const Ray &ray, Hit &hit) const;

	BVHBuildNode *recursiveBuild(std::vector<Object *>objects);
	void getSample(BVHBuildNode *node, float p, Hit &pos, float &pdf) const;
	void Sample(Hit &hit, float &pdf) const;

	BVHBuildNode *root;
	const int maxPrimsInNode;
	const SplitMethod splitMethod;
	std::vector<Object *> primitives;
};






#endif //RAYTRACING_BVH_H
