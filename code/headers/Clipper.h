#pragma once

// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <glm/glm.hpp>

namespace MGVisualizer
{

	using namespace glm;

	class Clipper
	{

	public:

		/// <summary>
		/// Clip vertices in an 800x600 window
		/// </summary>
		/// <param name="vertices">Pointer to first vertex of the polygon</param>
		/// <param name="first_index">Pointer to the index of the first vertex</param>
		/// <param name="last_index">Pointer to the index of the last vertex</param>
		/// <param name="clipped_vertices">Pointer to first element of array where clipped vertices are going to be storaged</param>
		/// <returns>Number of vertices of the clipped polygon</returns>
		static int clip(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices, int width, int height);

	private:

		static int clip_against_line(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices, float a, float b, float c);
		static ivec4 line_intersection(float a, float b, float c, const ivec4& point0, const ivec4& point1);

	};
}
