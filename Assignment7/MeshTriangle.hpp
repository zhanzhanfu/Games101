#pragma once

#include "Triangle.hpp"
#include "BVH.hpp"
#include "OBJ_Loader.hpp"
#include <cassert>
#include <array>

class MeshTriangle : public Object
{
public:
	Bounds3 bounding_box;
	std::unique_ptr<Vector3f[]> vertices;
	uint32_t numTriangles;
	std::unique_ptr<uint32_t[]> vertexIndex;
	std::unique_ptr<Vector2f[]> stCoordinates;

	std::vector<Triangle> triangles;

	BVHAccel *bvh;
	float area;

	Material *m;

	MeshTriangle(const std::string &filename, Material *mt = new Material())
	{
		objl::Loader loader;
		loader.LoadFile(filename);
		area = 0;
		m = mt;
		assert(loader.LoadedMeshes.size() == 1);
		auto mesh = loader.LoadedMeshes[0];

		Vector3f min_vert = Vector3f{ std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity() };
		Vector3f max_vert = Vector3f{ -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity() };
		for (int i = 0; i < mesh.Vertices.size(); i += 3)
		{
			std::array<Vector3f, 3> face_vertices;

			for (int j = 0; j < 3; j++)
			{
				auto vert = Vector3f(mesh.Vertices[i + j].Position.X,
					mesh.Vertices[i + j].Position.Y,
					mesh.Vertices[i + j].Position.Z);
				face_vertices[j] = vert;

				min_vert = Vector3f(std::min(min_vert.x, vert.x),
					std::min(min_vert.y, vert.y),
					std::min(min_vert.z, vert.z));
				max_vert = Vector3f(std::max(max_vert.x, vert.x),
					std::max(max_vert.y, vert.y),
					std::max(max_vert.z, vert.z));
			}

			triangles.emplace_back(face_vertices[0], face_vertices[1], face_vertices[2], mt);
		}

		bounding_box = Bounds3(min_vert, max_vert);

		std::vector<Object *> ptrs;
		for (auto &tri : triangles)
		{
			ptrs.push_back(&tri);
			area += tri.area;
		}
		bvh = new BVHAccel(ptrs);
	}
	bool intersect(const Ray &ray, Hit &hit) const override
	{
		//bool flag = false;
		//for (auto &tri : triangles)
		//{
		//	if (tri.intersect(ray, hit))
		//		flag = true;
		//}
		//return flag;
		return bvh->intersect(ray, hit);
	}

	Bounds3 getBounds() const override { return bounding_box; }


	void Sample(Hit &hit, float &pdf) const override
	{
		bvh->Sample(hit, pdf);
	}
	float getArea() const override
	{
		return area;
	}

	bool hasEmit() const override
	{
		return m->hasEmit();
	}

};