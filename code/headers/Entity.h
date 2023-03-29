
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include <vector>
#include <Color_Buffer.hpp>
#include "Transform.h"

namespace MGVisualizer
{

	using argb::Rgb888;
	using  std::vector;

	class Entity
	{

		// Define Color as Rgb888
		typedef Rgb888 Color;

	private:

		Entity* parent;

		Transform transform;

		// Mesh vectors
		vector< vec4  > original_vertices;
		vector< int   > original_indices;
		vector< Color > original_colors;
		vector< vec4  > transformed_vertices;
		vector< ivec4 > display_vertices;

	public:

		Entity(const char* model_path, vec3 position = vec3(), vec3 rotation = vec3(), vec3 scale = vec3(1.f, 1.f, 1.f), Entity* parent_entity = nullptr);

		Transform* get_transform() { return &transform; }

		vector< vec4  >* get_transformed_vertices() { return &transformed_vertices; }
		vector< ivec4 >* get_display_vertices    () { return     &display_vertices; }

		// Make const, find a way to iterate
		vector< int >* get_original_indices() { return  &original_indices; }

		const vector< vec4  >* get_original_vertices() { return &original_vertices; }
		const vector< Color >* get_original_colors  () { return   &original_colors; }

	private:

		void load_model(const char* model_path);

		// TODO: Erase this
		float rand_clamp() { return float(rand() & 0xff) * 0.0039215f; }

	};
}