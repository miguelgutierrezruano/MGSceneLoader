
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

	private:

		float fov;
		float nearPlane;
		float farPlane;

		float movementSpeed;
		float rotationSpeed;

	public:

		Camera();

		void move_camera_front(float delta)
		{  
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition - transform.get_forward() * movementSpeed * delta);
		}
		void  move_camera_back(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition + transform.get_forward() * movementSpeed * delta);
		}
		void  move_camera_left(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition - transform.get_right() * movementSpeed * delta);
		}
		void move_camera_right(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition + transform.get_right() * movementSpeed * delta);
		}

		void move_camera  (float delta, vec2 positionDifference);
		void rotate_camera(float delta, vec2 positionDifference);

		mat4 get_projection_matrix(float aspect_ratio);
	};
}
