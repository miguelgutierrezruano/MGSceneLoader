
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Camera.h"

namespace MGVisualizer
{
	Camera::Camera()
	{
		// Default camera settings
		fov = 20.f;
		nearPlane = 1.f;
		farPlane = 100.f;

#if _DEBUG
		movementSpeed = 4.f;
		rotationSpeed = 2.f;
#else
		movementSpeed = 70.f;
		rotationSpeed = 20.f;
#endif
	}

	void Camera::move_camera(float delta, vec2 positionDifference)
	{
		// Modify position over right vector
		vec3 lastPosition = transform.get_position();
		transform.set_position(lastPosition - transform.get_right() * movementSpeed * positionDifference.x * delta);

		// Modify position over up vector
		lastPosition = transform.get_position();
		transform.set_position(lastPosition - transform.get_up() * movementSpeed * positionDifference.y * delta);
	}

	void Camera::rotate_camera(float delta, vec2 positionDifference)
	{
		// Modify rotation over X axis
		vec3 lastRotation = transform.get_rotation();
		transform.set_rotation(vec3(lastRotation.x + positionDifference.y * rotationSpeed * delta, lastRotation.y, lastRotation.z));

		// Modify rotation over Y axis
		lastRotation = transform.get_rotation();
		transform.set_rotation(vec3(lastRotation.x, lastRotation.y - positionDifference.x * rotationSpeed * delta, lastRotation.z));
	}

	mat4 Camera::get_projection_matrix(float aspect_ratio)
	{
		return perspective(fov, aspect_ratio, nearPlane, farPlane);
	}
}