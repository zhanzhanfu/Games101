#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object *> p, int maxPrimsInNode, SplitMethod splitMethod)
	: primitives(std::move(p)), maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod), root(nullptr)
{
	time_t start, stop;
	time(&start);
	if (primitives.empty())
		return;

	root = recursiveBuild(primitives);

	time(&stop);
	double diff = difftime(stop, start);
	int hrs = (int)diff / 3600;
	int mins = ((int)diff / 60) - (hrs * 60);
	int secs = (int)diff - (hrs * 3600) - (mins * 60);

	printf("Complete, %d prims/node.\nTime Taken: %i hrs, %i mins, %i secs\n\n", maxPrimsInNode, hrs, mins, secs);
}

BVHBuildNode *BVHAccel::recursiveBuild(std::vector<Object *> objects)
{
	BVHBuildNode *node = new BVHBuildNode();

	// Compute bounds of all primitives in BVH node
	Bounds3 bounds;
	for (auto &object : objects)
		bounds = Union(bounds, object->getBounds());
	if (objects.size() == 1)
	{
		// Create leaf _BVHBuildNode_
		node->bounds = objects[0]->getBounds();
		node->object = objects[0];
		node->left = nullptr;
		node->right = nullptr;
		node->area = objects[0]->getArea();
		return node;
	}
	else if (objects.size() == 2)
	{
		node->left = recursiveBuild(std::vector{ objects[0] });
		node->right = recursiveBuild(std::vector{ objects[1] });

		node->bounds = Union(node->left->bounds, node->right->bounds);
		node->area = node->left->area + node->right->area;
		return node;
	}
	else
	{
		Bounds3 centroidBounds;
		for (auto &object : objects)
			centroidBounds = Union(centroidBounds, object->getBounds().Centroid());
		int dim = centroidBounds.maxExtent();
		std::sort(objects.begin(), objects.end(), 
			[dim](auto &f1, auto &f2) { return f1->getBounds().Centroid()[dim] < f2->getBounds().Centroid()[dim]; });

		auto beginning = objects.begin();
		auto middling = objects.begin() + (objects.size() / 2);
		auto ending = objects.end();

		auto leftshapes = std::vector<Object *>(beginning, middling);
		auto rightshapes = std::vector<Object *>(middling, ending);

		assert(objects.size() == (leftshapes.size() + rightshapes.size()));

		node->left = recursiveBuild(leftshapes);
		node->right = recursiveBuild(rightshapes);

		node->bounds = Union(node->left->bounds, node->right->bounds);
		node->area = node->left->area + node->right->area;
	}

	return node;
}

bool BVHAccel::intersect(const Ray &ray, Hit &hit) const
{
	return intersectNode(root, ray, hit);
}

bool BVHAccel::intersectNode(BVHBuildNode *node, const Ray &ray, Hit &hit) const
{
	// TODO Traverse the BVH to find intersection
	if (!node || !node->bounds.intersect(ray))
		return false;
	if (node->object)
	{
		return node->object->intersect(ray, hit);
	}
	bool f1 = intersectNode(node->left, ray, hit);
	bool f2 = intersectNode(node->right, ray, hit);
	return f1 || f2;
}


void BVHAccel::getSample(BVHBuildNode *node, float p, Hit &hit, float &pdf) const
{
	if (node->left == nullptr || node->right == nullptr)
	{
		node->object->Sample(hit, pdf);
		pdf *= node->area;
		return;
	}
	if (p < node->left->area) getSample(node->left, p, hit, pdf);
	else getSample(node->right, p - node->left->area, hit, pdf);
}

void BVHAccel::Sample(Hit &hit, float &pdf) const
{
	float p = std::sqrt(get_random_float()) * root->area;
	getSample(root, p, hit, pdf);
	pdf /= root->area;
}