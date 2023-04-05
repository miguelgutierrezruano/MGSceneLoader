
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <SFML/Window.hpp>
#include <Color_Buffer.hpp>
#include <glm/glm.hpp>
#include "Rasterizer.h"
#include "Entity.h"
#include "Camera.h"

namespace MGVisualizer
{
    using namespace sf;

	using  std::map;
    using argb::Rgb888;
    using argb::Color_Buffer;

    class View
    {
    private:

        typedef Rgb888                Color;
        typedef Color_Buffer< Color > Color_Buffer;

    private:

        Camera camera;
		map< std::string, Entity* > entities;

        Color_Buffer               color_buffer;
        Rasterizer< Color_Buffer > rasterizer;

        unsigned width;
        unsigned height;

    public:

        View(unsigned width, unsigned height);

        // Used in main loop
        void update();
        void render();
        void process_events(Event & sfEvent, float delta);

        // Used in entity render
        void set_rasterizer_color(Color color);
        void rasterizer_fill_polygon(const ivec4* const vertices,
            const int* const indices_begin,
            const int* const indices_end);
        
        bool  is_frontface(const vec4* const projected_vertices, const int* const indices);
    };
}