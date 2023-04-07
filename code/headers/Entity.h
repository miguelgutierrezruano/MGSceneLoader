
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include <vector>
#include <Color_Buffer.hpp>
#include "Transform.h"

namespace MGVisualizer
{
	class View;

	using argb::Rgb888;
	using  std::vector;

	class Entity
	{

		// Define Color as Rgb888
		typedef Rgb888 Color;

	private:

		Entity* parent;

		Transform transform;

		// Mesh vectors foreach mesh of the model
		vector< vector <  vec4 > > original_vertices;
		vector< vector <   int > > original_indices;
		vector< vector < Color > > original_colors;
		vector< vector <  vec4 > > transformed_vertices;
		vector< vector < ivec4 > > display_vertices;

	public:

		Entity(const char* model_path, Entity* parent_entity = nullptr, vec3 position = vec3(), vec3 rotation = vec3(), vec3 scale = vec3(1.f, 1.f, 1.f));

		Transform* get_transform() { return &transform; }
		Entity* get_parent() { return parent; }

		void update(mat4 projection);
		void render(mat4 transformation, View * view);

	private:

		void load_model(const char* model_path);
	};
}