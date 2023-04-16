
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
#include "DirectionalLight.h"

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

        unsigned width;
        unsigned height;

        Camera camera;

        // Map containing each entity
		map< std::string, Entity* > entities;

        // Vector with lights
        vector< Light* > lights;

        Color_Buffer               color_buffer;
        Rasterizer< Color_Buffer > rasterizer;

        glm::vec2 mouseLastPosition;

        float worldRotation;
        float cloudRotation;

    public:

        /// <summary>
        /// Create view to render entities
        /// </summary>
        /// <param name="width">Window width</param>
        /// <param name="height">Window height</param>
        View(unsigned width, unsigned height);

        /// <summary>
        /// Update view
        /// </summary>
        void update();

        /// <summary>
        /// Render every entity in the window
        /// </summary>
        void render();

        /// <summary>
        /// Process SFML events
        /// </summary>
        /// <param name="sfEvent">SFML event to process</param>
        /// <param name="delta">Time between frames</param>
        void process_events(Event & sfEvent, float delta);

        unsigned get_width()  { return  width; }
        unsigned get_height() { return height; }

        vector< Light* >& get_lights() { return lights; }

        /// <summary>
        /// Set views rasterizer color
        /// </summary>
        /// <param name="color">New color to render</param>
        void set_rasterizer_color(Color color);

        /// <summary>
        /// Call views rasterizer to render a polygon
        /// </summary>
        /// <param name="vertices">Pointer to first vertex</param>
        /// <param name="indices_begin">Pointer to first index</param>
        /// <param name="indices_end">Pointer to last index</param>
        void rasterizer_fill_polygon(const ivec4* const vertices,
            const int* const indices_begin,
            const int* const indices_end);
        
        /// <summary>
        /// Check if a given polygon is not facing to the camera
        /// </summary>
        /// <param name="projected_vertices">Pointer to first vertex to check</param>
        /// <param name="indices">Pointer to first index to check</param>
        /// <returns></returns>
        bool is_backface(const vec4* const projected_vertices, const int* const indices);
    };
}