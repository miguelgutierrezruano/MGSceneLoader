
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include <glm\glm.hpp>

using namespace glm;

namespace MGVisualizer
{
	class Transform
	{

	private:

		vec3 position;
		vec3 rotation;
		vec3 scale;

		mat4 transformMatrix;

	public:

		Transform()
		{
			position = vec3();
			rotation = vec3();
			scale = vec3();
		}

		Transform(const vec3 startPosition, const vec3 startRotation, const vec3 startScale)
		{
			position = startPosition;
			rotation = startRotation;
			scale = startScale;

			// Make matrix
		}

		const vec3 get_position() { return position; }
		const vec3 get_rotation() { return rotation; }
		const vec3 get_scale() { return scale; }

		void set_position(vec3 newPosition) { position = newPosition; }
		void set_rotation(vec3 newRotation) { rotation = newRotation; }
		void set_scale(vec3 newScale) { scale = newScale; }
	};
}