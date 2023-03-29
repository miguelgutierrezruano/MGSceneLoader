
// Este código es de dominio público.
// angel.rodriguez@esne.edu
// 2013.12 - 2021.04

#pragma once

#include <cstdlib>
#include <vector>
#include <Color_Buffer.hpp>
#include <glm/glm.hpp>
#include "Rasterizer.h"

namespace MGVisualizer
{
    using  std::vector;
    using argb::Rgb888;
    using argb::Color_Buffer;

    class View
    {
    private:

        typedef Rgb888                Color;
        typedef Color_Buffer< Color > Color_Buffer;

    private:

        static constexpr char mesh_file_path[] = "../binaries/stanford-bunny.obj";

        Color_Buffer               color_buffer;
        Rasterizer< Color_Buffer > rasterizer;

		// Converted to glm to avoid confusion
		vector< vec4 >  original_vertices;
		vector< int >   original_indices;
		vector< Color > original_colors;
		vector< vec4 >  transformed_vertices;
        vector< ivec4 > display_vertices;

        unsigned width;
        unsigned height;

    public:

        View(unsigned width, unsigned height);

        void update();
        void render();

    private:

        bool  is_frontface(const vec4* const projected_vertices, const int* const indices);
        float rand_clamp() { return float(rand() & 0xff) * 0.0039215f; }

    };
}