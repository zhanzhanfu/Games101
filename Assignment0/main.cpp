#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main()
{
	// Basic Example of cpp
	std::cout << "Example of cpp \n";
	float a = 1.0, b = 2.0;
	std::cout << a << std::endl;
	std::cout << a / b << std::endl;
	std::cout << std::sqrt(b) << std::endl;
	std::cout << std::acos(-1) << std::endl;
	std::cout << std::sin(30.0 / 180.0 * acos(-1)) << std::endl;

	// Example of vector
	std::cout << "Example of vector \n";
	// vector definition
	Eigen::Vector3f v(1.0f, 2.0f, 3.0f);
	Eigen::Vector3f w(1.0f, 0.0f, 0.0f);
	// vector output
	std::cout << "Example of output \n";
	std::cout << v << std::endl;
	// vector add
	std::cout << "Example of add \n";
	std::cout << v + w << std::endl;
	// vector scalar multiply
	std::cout << "Example of scalar multiply \n";
	std::cout << v * 3.0f << std::endl;
	std::cout << 2.0f * v << std::endl;

	// Example of matrix
	std::cout << "Example of matrix \n";
	// matrix definition
	Eigen::Matrix3f i, j;
	i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
	j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
	// matrix output
	std::cout << "Example of output \n";
	std::cout << i << std::endl;
	// matrix add i + j
	// matrix scalar multiply i * 2.0
	// matrix multiply i * j
	// matrix multiply vector i * v

	const float PI = 3.14159265;
	Eigen::Vector3f p(1, 1, 0);
	Eigen::Matrix3f r, t;
	float rad = 45 * PI / 180;
	r << cos(rad), -sin(rad), 0, sin(rad), cos(rad), 0, 0, 0, 1;
	t << 1, 0, 1, 0, 1, 2, 0, 0, 1;
	p = t * r * p;
	std::cout << "p: " << p << std::endl;

	//vector operator
	Eigen::Vector3f v1(1, 1, 0);
	Eigen::Vector3f v2(1, 0, 1);
	std::cout << "v1: " << v1 << std::endl;
	std::cout << "v1.norm(): " << v1.norm() << std::endl;
	std::cout << "v1.normalized(): " << v1.normalized() << std::endl;
	std::cout << "v1: " << v1 << std::endl;
	v1.normalize();
	std::cout << "v1: " << v1 << std::endl;
	std::cout << "v1 * v2: " << v1.cwiseProduct(v2) << std::endl;
	std::cout << "v1.dot(v2): " << v1.dot(v2) << std::endl;
	std::cout << "v1.cross(v2): " << v1.cross(v2) << std::endl;

	return 0;
}


