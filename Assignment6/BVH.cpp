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
	if (objects.size() <= maxPrimsInNode)
	{
		// Create leaf _BVHBuildNode_
		node->nPrimitives = objects.size();
		node->object = new Object * [node->nPrimitives];
		Bounds3 b;
		for (int i = 0; i < objects.size(); i++)
		{
			node->object[i] = objects[i];
			b = Union(b, objects[i]->getBounds());
		}
		node->bounds = b;
		node->left = nullptr;
		node->right = nullptr;
		return node;
	}
	else
	{
		switch (splitMethod)
		{
		case SplitMethod::NAIVE:
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
			break;
		}
		case SplitMethod::SAH:
		{
			float cost_min = std::numeric_limits<float>::infinity();
			int split = -1;
			int dim = -1;
			int num_buckets = 16;
			for (int d = 0; d < 3; d++) //three axis
			{
				std::vector<Bounds3> buckets(num_buckets);
				std::vector<int> num_p(num_buckets);
				for (auto &p : objects)
				{
					int b = bounds.Offset(p->getBounds().Centroid())[d] * num_buckets;
					buckets[b] = Union(buckets[b], p->getBounds());
					num_p[b]++;
				}
				for (int sp = 1; sp < num_buckets; sp++)
				{
					Bounds3 b_left, b_right;
					int num_left = 0, num_right = 0;
					for (int i = 0; i < sp; i++)
					{
						b_left = Union(b_left, buckets[i]);
						num_left += num_p[i];
					}
					for (int i = sp; i < num_buckets; i++)
					{
						b_right = Union(b_right, buckets[i]);
						num_right += num_p[i];
					}
					float cost = b_left.SurfaceArea() * num_left + b_right.SurfaceArea() * num_right;

					if (cost < cost_min)
					{
						cost_min = cost;
						split = sp;
						dim = d;
					}
				}
			}
			assert(split != -1 && dim != -1);
			std::sort(objects.begin(), objects.end(),
				[dim](auto &f1, auto &f2) { return f1->getBounds().Centroid()[dim] < f2->getBounds().Centroid()[dim]; });
			float threshold = bounds.pMin[dim] + bounds.Diagonal()[dim] * split / num_buckets;
			int mid = 0;
			for (; mid < objects.size(); mid++)
			{
				if (objects[mid]->getBounds().Centroid()[dim] > threshold)
					break;
			}
			auto leftshapes = std::vector<Object *>(objects.begin(), objects.begin() + mid);
			auto rightshapes = std::vector<Object *>(objects.begin() + mid, objects.end());
			assert(objects.size() == (leftshapes.size() + rightshapes.size()));

			node->left = recursiveBuild(leftshapes);
			node->right = recursiveBuild(rightshapes);
			node->bounds = Union(node->left->bounds, node->right->bounds);
			break;
		}
		}

	}

	return node;
}




Intersection BVHAccel::Intersect(const Ray &ray) const
{
	Intersection isect;
	if (!root)
		return isect;
	isect = getIntersection(root, ray);
	return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode *node, const Ray &ray) const
{
	// TODO Traverse the BVH to find intersection
	Intersection isect;
	std::array<int, 3> a;
	if (!node->bounds.IntersectP(ray, ray.direction_inv, a))
		return isect;
	if (node->nPrimitives > 0)
	{
		for (int i = 0; i < node->nPrimitives; i++)
		{
			Intersection t = node->object[i]->getIntersection(ray);
			if (t.distance < isect.distance)
				isect = t;
		}
		return isect;
	}
	Intersection is1 = getIntersection(node->left, ray);
	Intersection is2 = getIntersection(node->right, ray);
	return is1.distance < is2.distance ? is1 : is2;
}