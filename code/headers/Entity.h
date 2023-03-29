
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include "Transform.h"

namespace MGVisualizer
{
	class Entity
	{

	private:

		Entity* parent;

		Transform transform;

		// Original vertices and colors

	public:

		Entity(const char* model_path, vec3 position = vec3(), vec3 rotation = vec3(), vec3 scale = vec3(), Entity* parent_entity = nullptr);

		Transform* get_transform() { return &transform; }

	};
}