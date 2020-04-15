#include "rasterizer.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
using namespace std;
using namespace Eigen;

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
	frame_buf.resize(w * h);
	sample_buf.resize(4 * w * h);
	depth_buf.resize(4 * w * h);
}

rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
	auto id = get_next_id();
	pos_buf.emplace(id, positions);

	return { id };
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
	auto id = get_next_id();
	ind_buf.emplace(id, indices);

	return { id };
}


rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &colors)
{
	auto id = get_next_id();
	col_buf.emplace(id, colors);

	return { id };
}

void rst::rasterizer::clear(rst::Buffers buff)
{
	if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
	{
		std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0, 0, 255 });
		std::fill(sample_buf.begin(), sample_buf.end(), Eigen::Vector3f{ 0, 0, 1 });
	}
	if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
	{
		std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
	}
}


auto to_vec4(const Eigen::Vector3f &v3, float w = 1.0f)
{
	return Eigen::Vector4f(v3.x(), v3.y(), v3.z(), w);
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
	auto &buf = pos_buf[pos_buffer.pos_id];
	auto &ind = ind_buf[ind_buffer.ind_id];
	auto &col = col_buf[col_buffer.col_id];

	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;

	Eigen::Matrix4f mvp = projection * view * model;
	for (auto &i : ind)
	{
		Triangle t;
		Eigen::Vector4f v[] = {
				mvp * to_vec4(buf[i[0]], 1.0f),
				mvp * to_vec4(buf[i[1]], 1.0f),
				mvp * to_vec4(buf[i[2]], 1.0f)
		};
		//Homogeneous division
		for (auto &vec : v)
		{
			vec /= vec.w();
		}
		//Viewport transformation
		for (auto &vert : v)
		{
			vert.x() = 0.5 * width * (vert.x() + 1.0);
			vert.y() = 0.5 * height * (vert.y() + 1.0);
			vert.z() = vert.z() * f1 + f2;
		}

		for (int i = 0; i < 3; ++i)
		{
			t.setVertex(i, v[i].head<3>());
			t.setVertex(i, v[i].head<3>());
			t.setVertex(i, v[i].head<3>());
		}

		Eigen::Vector3f col_x = col[i[0]];
		Eigen::Vector3f col_y = col[i[1]];
		Eigen::Vector3f col_z = col[i[2]];

		t.setColor(0, col_x[0], col_x[1], col_x[2]);
		t.setColor(1, col_y[0], col_y[1], col_y[2]);
		t.setColor(2, col_z[0], col_z[1], col_z[2]);

		rasterize_triangle(t);
	}
}


static bool insideTriangle(float x, float y, const Vector3f *v)
{
	// TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
	int res[3] = {};
	Vector3f p(x, y, 1);
	res[0] = (v[1] - v[0]).cross(p - v[0]).z() > 0 ? 1 : -1;
	res[1] = (v[2] - v[1]).cross(p - v[1]).z() > 0 ? 1 : -1;
	res[2] = (v[0] - v[2]).cross(p - v[2]).z() > 0 ? 1 : -1;
	if (res[0] == res[1] && res[0] == res[2])
		return true;
	return false;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f *v)
{
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}


/*
void rst::rasterizer::rasterize_triangle(const Triangle &t)
{

	// TODO : Find out the bounding box of current triangle.
	// iterate through the pixel and find if the current pixel is inside the triangle
	// If so, use the following code to get the interpolated z value.
	//auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
	//float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	//float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	//z_interpolated *= w_reciprocal;
	// TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.

	std::vector<Eigen::Vector4f> v = t.toVector4();
	int min_x = min(min(v[0].x(), v[1].x()), v[2].x());
	int max_x = max(max(v[0].x(), v[1].x()), v[2].x());
	int min_y = min(min(v[0].y(), v[1].y()), v[2].y());
	int max_y = max(max(v[0].y(), v[1].y()), v[2].y());
	for (int x = min_x; x <= max_x; x++)
	{
		for (int y = min_y; y <= max_y; y++)
		{
			int ind = get_index(x, y);
			if (insideTriangle(x + 0.5, y + 0.5, t.v))
			{
				auto tup = computeBarycentric2D(x + 0.5, y + 0.5, t.v);
				float alpha = get<0>(tup);
				float beta = get<1>(tup);
				float gamma = get<2>(tup);
				float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
				float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
				z_interpolated *= w_reciprocal;
				Vector3f color = alpha * t.color[0] + beta * t.color[1] + gamma * t.color[2];
				if (z_interpolated < depth_buf[ind])
				{
					depth_buf[ind] = z_interpolated;
					set_pixel(x, y, color);
				}
			}
		}
	}
}
*/

void rst::rasterizer::rasterize_triangle(const Triangle &t)
{
	// TODO : Find out the bounding box of current triangle.
	// iterate through the pixel and find if the current pixel is inside the triangle
	// If so, use the following code to get the interpolated z value.
	//auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
	//float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	//float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	//z_interpolated *= w_reciprocal;
	// TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.

	std::vector<Eigen::Vector4f> v = t.toVector4();
	int min_x = min(min(v[0].x(), v[1].x()), v[2].x());
	int max_x = max(max(v[0].x(), v[1].x()), v[2].x());
	int min_y = min(min(v[0].y(), v[1].y()), v[2].y());
	int max_y = max(max(v[0].y(), v[1].y()), v[2].y());
	for (int x = min_x; x <= max_x; x++)
	{
		for (int y = min_y; y <= max_y; y++)
		{
			int ind = get_index(x, y) * 4;
			float x_offset[] = { 0.25, 0.75, 0.75, 0.25 };
			float y_offset[] = { 0.25, 0.25, 0.75, 0.75 };
			int num_samples = 4;
			int cnt = 0;
			for (int i = 0; i < 4; i++)
			{
				if (insideTriangle(x + x_offset[i], y + y_offset[i], t.v))
				{
					//depth
					auto tup = computeBarycentric2D(x + x_offset[i], y + y_offset[i], t.v);
					float alpha = get<0>(tup);
					float beta = get<1>(tup);
					float gamma = get<2>(tup);
					float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
					float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
					z_interpolated *= w_reciprocal;
					Vector3f color = alpha * t.color[0] + beta * t.color[1] + gamma * t.color[2];
					if (z_interpolated < depth_buf[ind + i])
					{
						cnt++;
						depth_buf[ind + i] = z_interpolated;
						sample_buf[ind + i] = color;
					}
				}
			}
			if (cnt > 0)
			{
				Vector3f res = { 0,0,0 };
				for (int i = 0; i < 4; i++)
				{
					res += sample_buf[ind + i];
				}
				res /= 4.0;
				set_pixel(x, y, res);
			}
		}
	}
}



void rst::rasterizer::set_pixel(int x, int y, const Eigen::Vector3f &color)
{
	if (x < 0 || x >= width || y < 0 || y >= height) return;

	frame_buf[get_index(x, y)] = color * 255.0f;
}

int rst::rasterizer::get_index(int x, int y)
{
	return (height - 1 - y) * width + x;
}















void rst::rasterizer::rasterize_wireframe(const Triangle &t)
{
	draw_line(t.c(), t.a());
	draw_line(t.c(), t.b());
	draw_line(t.b(), t.a());
}

// Bresenham's line drawing algorithm
// Code taken from a stack overflow answer: https://stackoverflow.com/a/16405254
void rst::rasterizer::draw_line(const Eigen::Vector3f &begin, const Eigen::Vector3f &end)
{
	auto x1 = begin.x();
	auto y1 = begin.y();
	auto x2 = end.x();
	auto y2 = end.y();

	Eigen::Vector3f line_color = { 255, 255, 255 };

	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = fabs(dx);
	dy1 = fabs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;

	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}
		set_pixel(x, y, line_color);
		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
			{
				px = px + 2 * dy1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					y = y + 1;
				}
				else
				{
					y = y - 1;
				}
				px = px + 2 * (dy1 - dx1);
			}
			//            delay(0);
			set_pixel(x, y, line_color);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		set_pixel(x, y, line_color);
		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
			{
				py = py + 2 * dx1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}
			//            delay(0);
			set_pixel(x, y, line_color);
		}
	}
}
