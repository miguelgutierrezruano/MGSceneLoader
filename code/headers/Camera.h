
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include "Transform.h"

namespace MGVisualizer
{

	// Perspective camera
	class Camera
	{

	public:

		Transform transform;

		float fov;
		float nearPlane;
		float farPlane;

	public:

		Camera();

		mat4 get_projection_matrix(float aspect_ratio);
	};
}
