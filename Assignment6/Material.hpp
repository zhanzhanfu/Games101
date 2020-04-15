#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

enum class MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Material
{
public:
	MaterialType m_type;
	Vector3f m_color;
	Vector3f m_emission;
	float ior;
	float Kd, Ks;
	float specularExponent;
	//Texture tex;

	inline Material(MaterialType t = MaterialType::DIFFUSE_AND_GLOSSY, Vector3f c = Vector3f(1, 1, 1), Vector3f e = Vector3f(0, 0, 0))
	{
		m_type = t;
		m_color = c;
		m_emission = e;
	}
	inline MaterialType getType() { return m_type; }
	inline Vector3f getColor() { return m_color; }
	inline Vector3f getColorAt(double u, double v) { return Vector3f(); }
	inline Vector3f getEmission() { return m_emission; }


};
#endif //RAYTRACING_MATERIAL_H
