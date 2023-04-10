#pragma once

// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <glm/glm.hpp>
#include "Light.h"

namespace MGVisualizer
{
	class DirectionalLight : public Light
	{

	private:

		// this or transform
		glm::vec3 direction;

	public:

		DirectionalLight(glm::vec3 initialDirection)
		{
			type = Directional;

			direction = normalize(initialDirection);
		}

	public:

		glm::vec3 get_direction() { return direction; }
	};
}
