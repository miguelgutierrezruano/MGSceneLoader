
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include "Transform.h"

namespace MGVisualizer
{

	/// <summary>
	/// Perspective Camera Class
	/// </summary>
	class Camera
	{

	public:

		/// Camera transform component
		Transform transform;

	private:

		// TODO: Getters and setters for private attributes
		float fov;
		float nearPlane;
		float farPlane;

		float movementSpeed;
		float rotationSpeed;

	public:

		Camera();

		/// <summary>
		/// Move camera forward in one frame
		/// </summary>
		/// <param name="delta">Time between frames</param>
		void move_camera_front(float delta)
		{  
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition + transform.get_forward() * movementSpeed * delta);
		}

		/// <summary>
		/// Move camera back in one frame
		/// </summary>
		/// <param name="delta">Time between frames</param>
		void  move_camera_back(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition - transform.get_forward() * movementSpeed * delta);
		}

		/// <summary>
		/// Move camera left in one frame
		/// </summary>
		/// <param name="delta">Time between frames</param>
		void  move_camera_left(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition - transform.get_right() * movementSpeed * delta);
		}

		/// <summary>
		/// Move camera right in one frame
		/// </summary>
		/// <param name="delta">Time between frames</param>
		void move_camera_right(float delta)
		{
			vec3 lastPosition = transform.get_position();
			transform.set_position(lastPosition + transform.get_right() * movementSpeed * delta);
		}

		/// <summary>
		/// Move camera along X and Y axis
		/// </summary>
		/// <param name="delta">Time between frames</param>
		/// <param name="positionDifference">Mouse position difference between frames</param>
		void move_camera  (float delta, vec2 positionDifference);

		/// <summary>
		/// Rotate camera along X and Y axis
		/// </summary>
		/// <param name="delta">Time between frames</param>
		/// <param name="positionDifference">Mouse position difference between frames</param>
		void rotate_camera(float delta, vec2 positionDifference);

		/// <summary>
		/// Get camera projection matrix
		/// </summary>
		/// <param name="aspect_ratio">Aspect ratio of window</param>
		/// <returns>Camera projection matrix</returns>
		mat4 get_projection_matrix(float aspect_ratio);
	};
}
