#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

enum class MaterialType { DIFFUSE };

class Material
{
private:

	// Compute reflection direction
	Vector3f reflect(const Vector3f &I, const Vector3f &N) const
	{
		return I - 2 * I.dot(N) * N;
	}

	// Compute refraction direction using Snell's law
	//
	// We need to handle with care the two possible situations:
	//
	//    - When the ray is inside the object
	//
	//    - When the ray is outside.
	//
	// If the ray is outside, you need to make cosi positive cosi = -N.I
	//
	// If the ray is inside, you need to invert the refractive indices and negate the normal N
	Vector3f refract(const Vector3f &I, const Vector3f &N, const float &ior) const
	{
		float cosi = clamp(-1, 1, I.dot(N));
		float etai = 1, etat = ior;
		Vector3f n = N;
		if (cosi < 0) { cosi = -cosi; }
		else { std::swap(etai, etat); n = -N; }
		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);
		return k < 0 ? 0 : eta * I + (eta * cosi - sqrt(k)) * n;
	}

	// Compute Fresnel equation
	//
	// \param I is the incident view direction
	//
	// \param N is the normal at the intersection point
	//
	// \param ior is the material refractive index
	//
	// \param[out] kr is the amount of light reflected
	void fresnel(const Vector3f &I, const Vector3f &N, const float &ior, float &kr) const
	{
		float cosi = clamp(-1, 1, I.dot(N));
		float etai = 1, etat = ior;
		if (cosi > 0) { std::swap(etai, etat); }
		// Compute sini using Snell's law
		float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
		// Total internal reflection
		if (sint >= 1)
		{
			kr = 1;
		}
		else
		{
			float cost = sqrtf(std::max(0.f, 1 - sint * sint));
			cosi = fabsf(cosi);
			float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
			float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
			kr = (Rs * Rs + Rp * Rp) / 2;
		}
		// As a consequence of the conservation of energy, transmittance is given by:
		// kt = 1 - kr;
	}

	Vector3f toWorld(const Vector3f &a, const Vector3f &N)
	{
		Vector3f B, C;
		if (std::fabs(N.x) > std::fabs(N.y))
		{
			float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
			C = Vector3f(N.z * invLen, 0.0f, -N.x * invLen);
		}
		else
		{
			float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
			C = Vector3f(0.0f, N.z * invLen, -N.y * invLen);
		}
		B = C.cross(N);
		return a.x * B + a.y * C + a.z * N;
	}

public:
	MaterialType m_type;
	Vector3f emit;
	float ior;
	Vector3f Kd, Ks;
	float specularExponent;
	//Texture tex;

	Material(MaterialType t = MaterialType::DIFFUSE, Vector3f e = Vector3f(0, 0, 0))
	{
		m_type = t;
		emit = e;
	}
	inline bool hasEmit() { return emit.length() > EPSILON ? true : false; }

	// sample a ray by Material properties
	inline Vector3f sample(const Vector3f &wi, const Vector3f &N);
	// given a ray, calculate the PdF of this ray
	inline float pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);
	// given a ray, calculate the contribution of this ray
	inline Vector3f eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);

};

Vector3f Material::sample(const Vector3f &wi, const Vector3f &N)
{
	switch (m_type)
	{
	case MaterialType::DIFFUSE:
	{
		// uniform sample on the hemisphere
		float x_1 = get_random_float(), x_2 = get_random_float();
		float z = std::fabs(1.0f - 2.0f * x_1);
		float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
		Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
		return toWorld(localRay, N);

		break;
	}
	}
}

float Material::pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N)
{
	switch (m_type)
	{
	case MaterialType::DIFFUSE:
	{
		// uniform sample probability 1 / (2 * PI)
		if (wo.dot(N) > 0.0f)
			return 0.5f / M_PI;
		else
			return 0.0f;
		break;
	}
	}
}

Vector3f Material::eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N)
{
	switch (m_type)
	{
	case MaterialType::DIFFUSE:
	{
		// calculate the contribution of diffuse model
		float cosalpha = wo.dot(N);
		if (cosalpha > 0.0f)
		{
			Vector3f diffuse = Kd / M_PI;
			return diffuse;
		}
		else
			return Vector3f(0.0f);
		break;
	}
	}
}

#endif //RAYTRACING_MATERIAL_H
