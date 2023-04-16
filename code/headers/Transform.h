
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

		mat4 transformationMatrix;

	public:

		Transform();

		Transform(const vec3 startPosition, const vec3 startRotation, const vec3 startScale);

		const vec3 get_position() { return position; }
		const vec3 get_rotation() { return rotation; }
		const vec3 get_scale() { return scale; }

		// Set parameters and update matrix
		void set_position(vec3 newPosition) { position = newPosition; update_matrix(); }
		void set_rotation(vec3 newRotation) { rotation = newRotation; update_matrix(); }
		void set_scale(vec3 newScale) { scale = newScale; update_matrix(); }

		void set_transformation(mat4 newTransformation);

		/// <summary>
		/// Get forward vector for this transform
		/// </summary>
		/// <returns>Normalized forward vector</returns>
		const vec3 get_forward();

		/// <summary>
		/// Get right vector for this transform
		/// </summary>
		/// <returns>Normalized right vector</returns>
		const vec3 get_right();

		/// <summary>
		/// Get up vector for this transform
		/// </summary>
		/// <returns>Normalized up vector</returns>
		const vec3 get_up();

		const mat4 get_matrix() { return transformationMatrix; }

	private:

		void update_matrix();
	};
}