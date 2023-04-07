
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Transform.h"

namespace MGVisualizer
{
	Transform::Transform()
	{
		position = vec3();
		rotation = vec3();
		scale = vec3(1.f, 1.f, 1.f);

		update_matrix();
	}

	Transform::Transform(const vec3 startPosition, const vec3 startRotation, const vec3 startScale)
	{
		position = startPosition;
		rotation = startRotation;
		scale = startScale;

		update_matrix();
	}

	const vec3 Transform::get_forward()
	{
		mat4 rotationMatrix = mat4(1);

		// Rotate based on euler angles
		rotationMatrix = glm::rotate(rotationMatrix, radians(rotation.x), vec3(1.f, 0.f, 0.f));
		rotationMatrix = glm::rotate(rotationMatrix, radians(rotation.y), vec3(0.f, 1.f, 0.f));
		rotationMatrix = glm::rotate(rotationMatrix, radians(rotation.z), vec3(0.f, 0.f, 1.f));

		vec4 forwardVector = vec4(0, 0, -1, 0);
		forwardVector = rotationMatrix * forwardVector;

		return vec3(forwardVector);
	}

	const vec3 Transform::get_right()
	{
		vec3 forward = get_forward();
		return normalize(cross(forward, vec3(0, 1, 0)));
	}

	const vec3 Transform::get_up()
	{
		vec3 forward = get_forward();
		vec3 right = get_right();
		return cross(right, forward);
	}

	void Transform::update_matrix()
	{
		mat4 identity = mat4(1);
		mat4 scalingMatrix = glm::scale(identity, scale);

		// Rotate based on euler angles
		mat4 xRotate = glm::rotate(identity, radians(rotation.x), vec3(1.f, 0.f, 0.f));
		mat4 yRotate = glm::rotate(identity, radians(rotation.y), vec3(0.f, 1.f, 0.f));
		mat4 zRotate = glm::rotate(identity, radians(rotation.z), vec3(0.f, 0.f, 1.f));

		mat4 rotationMatrix = zRotate * yRotate * xRotate;

		mat4 translationMatrix = glm::translate(identity, position);

		transformationMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	}
}