#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include "global.hpp"

class Texture
{
public:
	Texture(const std::string &name)
	{
		image_data = cv::imread(name);
		cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
		width = image_data.cols;
		height = image_data.rows;
	}

	Eigen::Vector3f getColor(float u, float v)
	{
		//assert(u >= 0 && u < 1 && v >= 0 && v < 1);
		u = std::min(u, 0.99f);
		u = std::max(u, 0.f);
		v = std::min(v, 0.99f);
		v = std::max(v, 0.f);
		auto u_img = u * width;
		auto v_img = (1 - v) * height;
		auto color = image_data.at<cv::Vec3b>(v_img, u_img);
		return Eigen::Vector3f(color[0], color[1], color[2]);
	}

	int width, height;
	cv::Mat image_data;
};
#endif //RASTERIZER_TEXTURE_H
