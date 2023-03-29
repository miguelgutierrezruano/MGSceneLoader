
// Este c�digo es de dominio p�blico.
// angel.rodriguez@esne.edu
// 2013.12 - 2021.04

#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <Color_Buffer.hpp>
#include <glm/glm.hpp>
#include "Rasterizer.h"
#include "Entity.h"

namespace MGVisualizer
{
	using  std::map;
    using argb::Rgb888;
    using argb::Color_Buffer;

    class View
    {
    private:

        typedef Rgb888                Color;
        typedef Color_Buffer< Color > Color_Buffer;

    private:

		map< std::string, Entity* > entities;

        Color_Buffer               color_buffer;
        Rasterizer< Color_Buffer > rasterizer;

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