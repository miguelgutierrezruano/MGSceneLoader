
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

		// Maybe has to change to proper controls
		void move_camera_front(float delta)
		{  
			vec3 lastPosition = transform.get_position();
			transform.set_position(vec3(lastPosition.x, lastPosition.y, lastPosition.z + movementSpeed * delta));
		}
		void  move_camera_back(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(vec3(lastPosition.x, lastPosition.y, lastPosition.z - movementSpeed * delta));
		}
		void  move_camera_left(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(vec3(lastPosition.x - movementSpeed * delta, lastPosition.y, lastPosition.z));
		}
		void move_camera_right(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(vec3(lastPosition.x + movementSpeed * delta, lastPosition.y, lastPosition.z));
		}

		void rotate_camera_up(float delta)
		{
			vec3 lastRotation = transform.get_rotation();
			transform.set_rotation(vec3(lastRotation.x + rotationSpeed * delta, lastRotation.y, lastRotation.z));
		}
		void  rotate_camera_down(float delta) 
		{
			vec3 lastRotation = transform.get_rotation();
			transform.set_rotation(vec3(lastRotation.x - rotationSpeed * delta, lastRotation.y, lastRotation.z));
		}
		void  rotate_camera_left(float delta)
		{
			vec3 lastRotation = transform.get_rotation();
			transform.set_rotation(vec3(lastRotation.x, lastRotation.y - rotationSpeed * delta, lastRotation.z));
		}
		void rotate_camera_right(float delta)
		{
			vec3 lastRotation = transform.get_rotation();
			transform.set_rotation(vec3(lastRotation.x, lastRotation.y + rotationSpeed * delta, lastRotation.z));
		}

		mat4 get_projection_matrix(float aspect_ratio);
	};
}
