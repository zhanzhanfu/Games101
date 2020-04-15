#pragma once

#include <eigen3/Eigen/Eigen>
#include <algorithm>
#include "Triangle.hpp"
#include "Shader.hpp"

namespace rst
{
	enum class Buffers
	{
		Color = 1,
		Depth = 2
	};

	inline Buffers operator|(Buffers a, Buffers b)
	{
		return Buffers((int)a | (int)b);
	}

	inline Buffers operator&(Buffers a, Buffers b)
	{
		return Buffers((int)a & (int)b);
	}

	enum class Primitive
	{
		Line,
		Triangle
	};

	/*
	 * For the curious : The draw function takes two buffer id's as its arguments.
	 * These two structs make sure that if you mix up with their orders, the
	 * compiler won't compile it. Aka : Type safety
	 * */
	struct pos_buf_id
	{
		int pos_id = 0;
	};

	struct ind_buf_id
	{
		int ind_id = 0;
	};

	struct col_buf_id
	{
		int col_id = 0;
	};

	class rasterizer
	{
	public:
		rasterizer(int w, int h);
		pos_buf_id load_positions(const std::vector<Eigen::Vector3f> &positions);
		ind_buf_id load_indices(const std::vector<Eigen::Vector3i> &indices);
		col_buf_id load_colors(const std::vector<Eigen::Vector3f> &colors);
		col_buf_id load_normals(const std::vector<Eigen::Vector3f> &normals);

		void set_model(const Eigen::Matrix4f &m) { model = m; }
		void set_view(const Eigen::Matrix4f &v) { view = v; }
		void set_projection(const Eigen::Matrix4f &p) { projection = p; }
		void set_texture(std::shared_ptr<Texture> tex) { texture = tex; }

		void set_vertex_shader(std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader) { vertex_shader = vert_shader; }
		void set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader) { fragment_shader = frag_shader; }

		void clear(Buffers buff);

		//void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);
		void draw(const std::vector<Triangle *> &TriangleList);
		void set_pixel(int x, int y, const Eigen::Vector3f &color);
		int get_index(int x, int y);

		std::vector<Eigen::Vector3f> &frame_buffer() { return frame_buf; }

	private:
		void rasterize_triangle(const Triangle &t, const std::vector<Eigen::Vector3f> &viewspace_pos);
		// VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

	private:
		int width, height;
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;

		std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
		std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
		std::map<int, std::vector<Eigen::Vector3f>> col_buf;
		std::map<int, std::vector<Eigen::Vector3f>> nor_buf;
		int normal_id = -1;

		std::vector<Eigen::Vector3f> frame_buf;
		std::vector<Eigen::Vector3f> sample_buf;
		std::vector<float> depth_buf;

		std::shared_ptr<Texture> texture;

		std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader;
		std::function<Eigen::Vector3f(vertex_shader_payload)> vertex_shader;
		

		int next_id = 0;
		int get_next_id() { return next_id++; }
	};
} // namespace rst
