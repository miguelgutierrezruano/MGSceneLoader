
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
        entities["deer"]->get_transform()->set_scale(vec3(0.015f, 0.015f, 0.015f));
    }

    void View::update()
    {
        // Create projection matrix

		static float angle = 0.f;

		angle++;

		entities["rabbit"]->get_transform()->set_rotation(vec3(180, angle, 0.f));

        mat4 projection = perspective(40.f, float(width) / height, 0.5f, 100.f);

        // Update each entity
        for (auto& [name, entity] : entities)
        {
            // Apply parent and projection transformations

            mat4 parentMatrix = mat4(1);

            // Get parent matrix
            if (entity->get_parent() != nullptr)
            {
                Entity* parent = entity->get_parent();
                parentMatrix *= parent->get_transform()->get_matrix();

                while (parent != nullptr)
                {
                    parent = parent->get_parent();

                    if(parent != nullptr)
                        parentMatrix *= parent->get_transform()->get_matrix();
                }
            }

            mat4 transformation = projection * parentMatrix * entity->get_transform()->get_matrix();

            size_t number_of_vertices = entity->get_original_vertices()->size();

            // Transform every vertex by transformation matrix
            for (size_t index = 0; index < number_of_vertices; index++)
            {
                // Save transformed vertex in transformed vertices vector
                vec4& vertex = entity->get_transformed_vertices()->at(index) =
                    transformation * entity->get_original_vertices()->at(index);

                // La matriz de proyección en perspectiva hace que el último componente del vector
                // transformado no tenga valor 1.0, por lo que hay que normalizarlo dividiendo:

                float divisor = 1.f / vertex.w;

                vertex.x *= divisor;
                vertex.y *= divisor;
                vertex.z *= divisor;
                vertex.w = 1.f;
            }
        }
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
        {
            size_t number_of_vertices = entity->get_transformed_vertices()->size();

            for (size_t index = 0; index < number_of_vertices; index++)
            {
                entity->get_display_vertices()->at(index) =
                    ivec4(transformation * entity->get_transformed_vertices()->at(index));
            }

            // Create size pointers
            int* indices = entity->get_original_indices()->data();
            int* end = indices + entity->get_original_indices()->size();

            // Make const iterator to do this
            for (; indices < end; indices += 3)
            {
                if (is_frontface(entity->get_transformed_vertices()->data(), indices))
                {
                    // Se establece el color del polígono a partir del color de su primer vértice:

                    rasterizer.set_color(entity->get_original_colors()->at(*indices));

                    // TODO: Clip vertices

                    rasterizer.fill_convex_polygon_z_buffer(entity->get_display_vertices()->data(), indices, indices + 3);
                }
            }
        }

        // Se copia el framebúffer oculto en el framebúffer de la ventana:

        color_buffer.blit_to_window();
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