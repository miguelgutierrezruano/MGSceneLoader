
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace MGVisualizer
{
	class Transform
	{

	private:

		vec3 position;
		vec3 rotation;
		vec3 scale;

	public:

		Transform()
		{
			position = vec3();
			rotation = vec3();
			scale = vec3(1.f, 1.f, 1.f);
		}

		Transform(const vec3 startPosition, const vec3 startRotation, const vec3 startScale)
		{
			position = startPosition;
			rotation = startRotation;
			scale = startScale;
		}

		const vec3 get_position() { return position; }
		const vec3 get_rotation() { return rotation; }
		const vec3 get_scale() { return scale; }

		void set_position(vec3 newPosition) { position = newPosition; }
		void set_rotation(vec3 newRotation) { rotation = newRotation; }
		void set_scale(vec3 newScale) { scale = newScale; }

		// Make matrix out of scale, rotate and translate
		mat4 get_matrix()
		{
			mat4 identity = mat4(1);
			mat4 scalingMatrix = glm::scale(identity, scale);
			
			// Rotate based on euler angles
			mat4 xRotate = glm::rotate(identity, rotation.x, vec3(1.f, 0.f, 0.f));
			mat4 yRotate = glm::rotate(identity, rotation.y, vec3(0.f, 1.f, 0.f));
			mat4 zRotate = glm::rotate(identity, rotation.z, vec3(0.f, 0.f, 1.f));

			mat4 rotationMatrix = zRotate * yRotate * xRotate;

			mat4 translationMatrix = glm::translate(identity, position);

			return translationMatrix * rotationMatrix * scalingMatrix;
		}
	};
}