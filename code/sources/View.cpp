
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <cassert>
#include <cmath>
#include "View.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace glm;

namespace MGVisualizer
{
    // Initialize view attributes
    View::View(unsigned width, unsigned height)
        :
        width(width),
        height(height),
        color_buffer(width, height),
        rasterizer(color_buffer)
    { 
        Entity* rabbit = new Entity("../binaries/stanford-bunny.obj");
        entities.emplace("rabbit", rabbit);

        entities["rabbit"]->get_transform()->set_position(vec3(0.f, 0.f, 10.f));
        entities["rabbit"]->get_transform()->set_rotation(vec3(180, 180, 0.f));
        entities["rabbit"]->get_transform()->set_scale(vec3(2.f, 2.f, 2.f));

        Entity* rabbit2 = new Entity("../binaries/stanford-bunny.obj", rabbit);
        entities.emplace("rabbit2", rabbit2);
        entities["rabbit2"]->get_transform()->set_position(vec3(-2.f, 0.f, 0.f));

        Entity* deer = new Entity("../binaries/deer.obj");
        entities.emplace("deer", deer);

        entities["deer"]->get_transform()->set_position(vec3(-20.f, -20.f, 40.f));
        entities["deer"]->get_transform()->set_rotation(vec3(180, 90, 0.f));
        entities["deer"]->get_transform()->set_scale(vec3(0.02f, 0.02f, 0.02f));
    }

    void View::update()
    {
        // Create projection matrix

		static float angle = 0.f;

		angle++;

		entities["rabbit"]->get_transform()->set_rotation(vec3(180, angle, 0.f));

        // TODO: Apply camera inverse transform to projection matrix
        mat4 projection = perspective(40.f, float(width) / height, 0.5f, 100.f);

        // Update each entity
        for (auto& [name, entity] : entities)
            entity->update(projection);
    }

    void View::render()
    {
        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
        // La coordenada Z se escala a un valor suficientemente grande dentro del
        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).

        mat4 identity(1);
        mat4 scaling = scale(identity, glm::vec3(float(width / 2), float(height / 2), 1.f));
        mat4 translation = translate(identity, glm::vec3(float(width / 2), float(height / 2), 0.f));
        mat4 transformation = translation * scaling;

        rasterizer.clear();

        // Render each entity
        for (auto& [name, entity] : entities)
            entity->render(transformation, this);

        // Se copia el framebúffer oculto en el framebúffer de la ventana:

        color_buffer.blit_to_window();
    }

    void View::set_rasterizer_color(Color color)
    {
        rasterizer.set_color(color);
    }

    void View::rasterizer_fill_polygon(const ivec4* const vertices, const int* const indices_begin, const int* const indices_end)
    {
        rasterizer.fill_convex_polygon_z_buffer(vertices, indices_begin, indices_end);
    }

    bool View::is_frontface(const vec4* const projected_vertices, const int* const indices)
    {
        const vec4& v0 = projected_vertices[indices[0]];
        const vec4& v1 = projected_vertices[indices[1]];
        const vec4& v2 = projected_vertices[indices[2]];

        // Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
        // Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:

        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
    }
}