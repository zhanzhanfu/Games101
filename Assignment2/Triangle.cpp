#include "Triangle.hpp"
#include <algorithm>
#include <stdexcept>

using namespace Eigen;

Triangle::Triangle()
{
	v[0] << 0, 0, 0;
	v[1] << 0, 0, 0;
	v[2] << 0, 0, 0;

	color[0] << 0.0, 0.0, 0.0;
	color[1] << 0.0, 0.0, 0.0;
	color[2] << 0.0, 0.0, 0.0;

	tex_coords[0] << 0.0, 0.0;
	tex_coords[1] << 0.0, 0.0;
	tex_coords[2] << 0.0, 0.0;
}

void Triangle::setVertex(int ind, const Vector3f &ver) { v[ind] = ver; }

void Triangle::setNormal(int ind, const Vector3f &n) { normal[ind] = n; }

void Triangle::setColor(int ind, float r, float g, float b)
{
	if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) || (b > 255.))
	{
		throw std::runtime_error("Invalid color values");
	}

	color[ind] = Vector3f((float)r / 255., (float)g / 255., (float)b / 255.);
}
void Triangle::setTexCoord(int ind, float s, float t)
{
	tex_coords[ind] = Vector2f(s, t);
}


std::vector<Eigen::Vector4f> Triangle::toVector4() const
{
	std::vector<Vector4f> res(3);
	std::transform(std::begin(v), std::end(v), res.begin(), [](auto &vec) {return Eigen::Vector4f(vec.x(), vec.y(), vec.z(), 1.f); });
	return std::move(res);
}

