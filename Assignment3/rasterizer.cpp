#include "rasterizer.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
using namespace std;

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

rst::col_buf_id rst::rasterizer::load_normals(const std::vector<Eigen::Vector3f> &normals)
{
	auto id = get_next_id();
	nor_buf.emplace(id, normals);
	normal_id = id;
	return { id };
}

void rst::rasterizer::clear(rst::Buffers buff)
{
	if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
	{
		std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0, 0, 0 });
		std::fill(sample_buf.begin(), sample_buf.end(), Eigen::Vector3f{ 0, 0, 0 });
	}
	if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
	{
		std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
	}
}


auto to_vec4(const Eigen::Vector3f &v3, float w)
{
	return Eigen::Vector4f(v3.x(), v3.y(), v3.z(), w);
}
//
//void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
//{
//	auto &buf = pos_buf[pos_buffer.pos_id];
//	auto &ind = ind_buf[ind_buffer.ind_id];
//	auto &col = col_buf[col_buffer.col_id];
//
//	float f1 = (50 - 0.1) / 2.0;
//	float f2 = (50 + 0.1) / 2.0;
//
//	Eigen::Matrix4f mvp = projection * view * model;
//	for (auto &i : ind)
//	{
//		Triangle t;
//		Eigen::Vector4f v[] = {
//				mvp * to_vec4(buf[i[0]], 1.0f),
//				mvp * to_vec4(buf[i[1]], 1.0f),
//				mvp * to_vec4(buf[i[2]], 1.0f)
//		};
//		//Homogeneous division
//		for (auto &vec : v)
//		{
//			vec /= vec.w();
//		}
//		//Viewport transformation
//		for (auto &vert : v)
//		{
//			vert.x() = 0.5 * width * (vert.x() + 1.0);
//			vert.y() = 0.5 * height * (vert.y() + 1.0);
//			vert.z() = vert.z() * f1 + f2;
//		}
//
//		for (int i = 0; i < 3; ++i)
//		{
//			t.setVertex(i, v[i].head<3>());
//			t.setVertex(i, v[i].head<3>());
//			t.setVertex(i, v[i].head<3>());
//		}
//
//		Eigen::Vector3f col_x = col[i[0]];
//		Eigen::Vector3f col_y = col[i[1]];
//		Eigen::Vector3f col_z = col[i[2]];
//
//		t.setColor(0, col_x[0], col_x[1], col_x[2]);
//		t.setColor(1, col_y[0], col_y[1], col_y[2]);
//		t.setColor(2, col_z[0], col_z[1], col_z[2]);
//
//		rasterize_triangle(t);
//	}
//}


void rst::rasterizer::draw(const std::vector<Triangle *> &TriangleList)
{
	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;

	Eigen::Matrix4f mvp = projection * view * model;
	for (const auto &t : TriangleList)
	{
		Triangle newtri(*t);

		std::vector<Eigen::Vector4f> mm{
				(view * model * t->v[0]),
				(view * model * t->v[1]),
				(view * model * t->v[2])
		};

		std::vector<Eigen::Vector3f> viewspace_pos(3);

		std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto &v) {return v.head<3>(); });

		Eigen::Vector4f v[] = {
				mvp * t->v[0],
				mvp * t->v[1],
				mvp * t->v[2]
		};
		//Homogeneous division
		for (auto &vec : v)
		{
			vec.x() /= vec.w();
			vec.y() /= vec.w();
			vec.z() /= vec.w();
		}

		Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
		Eigen::Vector4f n[] = {
				inv_trans * to_vec4(t->normal[0], 0.0f),
				inv_trans * to_vec4(t->normal[1], 0.0f),
				inv_trans * to_vec4(t->normal[2], 0.0f)
		};

		//Viewport transformation
		for (auto &vert : v)
		{
			vert.x() = 0.5 * width * (vert.x() + 1.0);
			vert.y() = 0.5 * height * (vert.y() + 1.0);
			vert.z() = vert.z() * f1 + f2;
		}

		for (int i = 0; i < 3; ++i)
		{
			//screen space coordinates
			newtri.setVertex(i, v[i]);
		}

		for (int i = 0; i < 3; ++i)
		{
			//view space normal
			newtri.setNormal(i, n[i].head<3>());
		}

		newtri.setColor(0, 148, 121.0, 92.0);
		newtri.setColor(1, 148, 121.0, 92.0);
		newtri.setColor(2, 148, 121.0, 92.0);

		// Also pass view space vertice position
		rasterize_triangle(newtri, viewspace_pos);
	}
}

static bool insideTriangle(float x, float y, const vector<Eigen::Vector3f> &v)
{
	// TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
	int res[3] = {};
	Eigen::Vector3f p(x, y, 1);
	res[0] = (v[1] - v[0]).cross(p - v[0]).z() > 0 ? 1 : -1;
	res[1] = (v[2] - v[1]).cross(p - v[1]).z() > 0 ? 1 : -1;
	res[2] = (v[0] - v[2]).cross(p - v[2]).z() > 0 ? 1 : -1;
	if (res[0] == res[1] && res[0] == res[2])
		return true;
	return false;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const vector<Eigen::Vector3f> &v)
{
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}


void rst::rasterizer::rasterize_triangle(const Triangle &t, const vector<Eigen::Vector3f> &viewspace_pos)
{
	// TODO: From your HW3, get the triangle rasterization code.
	// TODO: Inside your rasterization loop:
	//    * v[i].w() is the vertex view space depth value z.
	//    * Z is interpolated view space depth for the current pixel
	//    * zp is depth between zNear and zFar, used for z-buffer

	// float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	// float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	// zp *= Z;

	// TODO: Interpolate the attributes:
	// auto interpolated_color
	// auto interpolated_normal
	// auto interpolated_texcoords
	// auto interpolated_shadingcoords

	// Use: fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
	// Use: payload.view_pos = interpolated_shadingcoords;
	// Use: Instead of passing the triangle's color directly to the frame buffer, pass the color to the shaders first to get the final color;
	// Use: auto pixel_color = fragment_shader(payload);

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
			auto tup = computeBarycentric2D(x + 0.5, y + 0.5, t.toVector3());
			float alpha = get<0>(tup);
			float beta = get<1>(tup);
			float gamma = get<2>(tup);
			//sample
			float x_offset[] = { 0.25, 0.75, 0.75, 0.25 };
			float y_offset[] = { 0.25, 0.25, 0.75, 0.75 };
			int num_samples = 4;
			int cnt = 0;
			for (int i = 0; i < 4; i++)
			{
				if (insideTriangle(x + x_offset[i], y + y_offset[i], t.toVector3()))
				{
					float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
					float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
					zp *= Z;
					Eigen::Vector3f color = alpha * t.color[0] + beta * t.color[1] + gamma * t.color[2];
					if (zp < depth_buf[ind + i])
					{
						cnt++;
						depth_buf[ind + i] = zp;
						sample_buf[ind + i] = color;
					}
				}
			}
			if (cnt > 0)
			{
				Eigen::Vector3f color = { 0,0,0 };
				for (int i = 0; i < 4; i++)
				{
					color += sample_buf[ind + i];
				}
				color /= 4.0f;
				Eigen::Vector3f interpolated_normal = alpha * t.normal[0] + beta * t.normal[1] + gamma * t.normal[2];
				Eigen::Vector2f interpolated_texcoords = alpha * t.tex_coords[0] + beta * t.tex_coords[1] + gamma * t.tex_coords[2];
				fragment_shader_payload payload(color, interpolated_normal.normalized(), interpolated_texcoords, texture);
				Eigen::Vector3f interpolated_shadingcoords = alpha * viewspace_pos[0] + beta * viewspace_pos[1] + gamma * viewspace_pos[2];
				payload.view_pos = interpolated_shadingcoords;
				set_pixel(x, y, fragment_shader(payload));
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

