
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

	// Acting as a data container, entity controls everything
	class Mesh
	{

		// Define Color as Rgb888
		typedef Rgb888 Color;

	public:

		Transform transform;

		vector <  vec4 > original_vertices;
		vector <   int > original_indices;
		vector < Color > original_colors;
		vector <  vec4 > original_normals;

		vector <  vec4 > transformed_vertices;
		vector < ivec4 > display_vertices;

		vector < Color > computed_colors;
		// Transformed normals

	public:

		Mesh() { }
	};
}
