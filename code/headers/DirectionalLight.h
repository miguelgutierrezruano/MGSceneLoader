#pragma once

// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <glm/glm.hpp>
#include "Light.h"

namespace MGVisualizer
{
	/// <summary>
	/// Light coming from a specific direction
	/// </summary>
	class DirectionalLight : public Light
	{

	private:

		glm::vec3 direction;

	public:

		/// <summary>
		/// Constructor of directional light
		/// </summary>
		/// <param name="initialDirection">Direction the light comes from</param>
		DirectionalLight(glm::vec3 initialDirection)
		{
			type = Directional;

			direction = normalize(initialDirection);
		}

	public:

		glm::vec3 get_direction() { return direction; }
	};
}
