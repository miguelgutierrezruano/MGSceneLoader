
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Camera.h"

namespace MGVisualizer
{
	Camera::Camera()
	{
		fov = 40.f;
		nearPlane = 1.f;
		farPlane = 100.f;
	}

	mat4 Camera::get_projection_matrix(float aspect_ratio)
	{
		return perspective(fov, aspect_ratio, nearPlane, farPlane);
	}
}