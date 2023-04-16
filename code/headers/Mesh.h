
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

	/// <summary>
	/// Data container of every attributes a model needs to be rendered
	/// </summary>
	class Mesh
	{

		// Define Color as Rgb888
		typedef Rgb888 Color;

	public:

		/// <summary>
		/// Transform of mesh
		/// </summary>
		Transform transform;

		/// <summary>
		/// Model coordinates vertices
		/// </summary>
		vector <  vec4 > original_vertices;

		/// <summary>
		/// Indices to give vertex their connections
		/// </summary>
		vector <   int > original_indices;

		/// <summary>
		/// Original color of each vertex
		/// </summary>
		vector < Color > original_colors;

		/// <summary>
		/// Model coordinates normals
		/// </summary>
		vector <  vec4 > original_normals;

		/// <summary>
		/// Projected vertices
		/// </summary>
		vector <  vec4 > transformed_vertices;

		/// <summary>
		/// World space normals
		/// </summary>
		vector <  vec4 > transformed_normals;

		/// <summary>
		/// View space vertices
		/// </summary>
		vector < ivec4 > display_vertices;

		/// <summary>
		/// Colors after applying illumination
		/// </summary>
		vector < Color > computed_colors;

	public:

		Mesh() { }
	};
}
