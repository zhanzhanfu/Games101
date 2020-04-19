#pragma once
#ifndef RAYTRACING_OBJECT_H
#define RAYTRACING_OBJECT_H

#include "Vector.hpp"
#include "global.hpp"
#include "Bounds3.hpp"
#include "Ray.hpp"
#include "Hit.hpp"

class Object
{
public:
	Object() = default;
	virtual ~Object() = default;
	virtual bool intersect(const Ray &ray, Hit &hit) const = 0;
	virtual Bounds3 getBounds() const = 0;
	virtual float getArea() const = 0;
	virtual void Sample(Hit &hit, float &pdf) const  = 0;
	virtual bool hasEmit() const = 0;
};



#endif //RAYTRACING_OBJECT_H
