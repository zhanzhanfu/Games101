#ifndef RASTERIZER_TRIANGLE_H
#define RASTERIZER_TRIANGLE_H

#include <eigen3/Eigen/Eigen>
#include "Texture.hpp"
class Triangle
{
public:
	Triangle()
	{
		v[0] << 0, 0, 0, 0;
		v[1] << 0, 0, 0, 0;
		v[2] << 0, 0, 0, 0;

		color[0] << 0.0, 0.0, 0.0;
		color[1] << 0.0, 0.0, 0.0;
		color[2] << 0.0, 0.0, 0.0;

		tex_coords[0] << 0.0, 0.0;
		tex_coords[1] << 0.0, 0.0;
		tex_coords[2] << 0.0, 0.0;

		normal[0] << 0.0, 0.0, 0.0;
		normal[1] << 0.0, 0.0, 0.0;
		normal[2] << 0.0, 0.0, 0.0;
	}

	void setVertex(int ind, const Eigen::Vector4f &ver) { v[ind] = ver; }
	void setNormal(int ind, const Eigen::Vector3f &n) { normal[ind] = n; }
	void setTexCoord(int ind, const Eigen::Vector2f &tc) { tex_coords[ind] = tc; }
	void setColor(int ind, float r, float g, float b)
	{
		if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) || (b > 255.))
		{
			throw std::runtime_error("Invalid color values");
		}

		color[ind] = Eigen::Vector3f((float)r / 255., (float)g / 255., (float)b / 255.);
	}

	std::vector<Eigen::Vector3f> toVector3() const
	{
		std::vector<Eigen::Vector3f> res;
		res.emplace_back(v[0].x(), v[0].y(), v[0].z());
		res.emplace_back(v[1].x(), v[1].y(), v[1].z());
		res.emplace_back(v[2].x(), v[2].y(), v[2].z());
		return std::move(res);
	}

	std::vector<Eigen::Vector4f> toVector4() const
	{
		std::vector<Eigen::Vector4f> res{ v[0], v[1], v[2] };
		return std::move(res);
	}


	Eigen::Vector4f v[3];
	Eigen::Vector3f color[3];
	Eigen::Vector2f tex_coords[3];
	Eigen::Vector3f normal[3];
	std::shared_ptr<Texture> texture;
};

#endif // RASTERIZER_TRIANGLE_H
