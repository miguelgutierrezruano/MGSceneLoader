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

		static int clip(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices);

	private:

		static int clip_against_line(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices, float a, float b, float c);
		static ivec4 line_intersection(float a, float b, float c, const ivec4& point0, const ivec4& point1);

	};
}
