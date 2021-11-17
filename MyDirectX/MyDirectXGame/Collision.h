#pragma once
#include "CollisionPrimitive.h"

class Collision
{
public:
	static bool ChenkSphere2Sphere(float x1, float y1, float z1, float x2, float y2, float z2, float r1, float r2);

	static bool ChenkSphere2Plane(const Sphere &sphere, const Plane &plane, DirectX::XMVECTOR *inter = nullptr);

	static void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR &point, const Triangle &triangle, DirectX::XMVECTOR *closest);

	static bool CheckSpere2Triangle(const Sphere &sphere, const Triangle &triangle, DirectX::XMVECTOR *inter = nullptr);

	static bool CheckRay2Plane(const Ray &ray, const Plane &plane, float *distance = nullptr, DirectX::XMVECTOR *inter = nullptr);
};