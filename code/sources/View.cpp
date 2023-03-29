
// Este código es de dominio público.
// angel.rodriguez@esne.edu
// 2013.12 - 2021.04

#include <cassert>
#include <cmath>
#include "View.h"
#include "math.hpp"

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
    }

    void View::update()
    {
        // Se actualizan los parámetros de transformatión (sólo se modifica el ángulo):

        static float angle = 0.f;

        angle += 0.025f;

        // Se crean las matrices de transformación:

        mat4 identity(1);
        mat4 scaling = scale(identity, vec3(4, 4, 4));
        mat4 rotation_y = identity;
        mat4 translation = translate(identity, vec3(0.f, 0.5f, -10.f));
        mat4 projection = perspective(20.f, 1.f, 15.f, float(width) / height);

        // Creación de la matriz de transformación unificada:

        mat4 transformation = projection * translation * rotation_y * scaling;

        // Se transforman todos los vértices usando la matriz de transformación resultante:
        size_t number_of_vertices = entities["rabbit"]->get_original_vertices()->size();

        for (size_t index = 0; index < number_of_vertices; index++)
        {
            // Se multiplican todos los vértices originales con la matriz de transformación y
            // se guarda el resultado en otro vertex buffer:

            vec4& vertex = entities["rabbit"]->get_transformed_vertices()->at(index) = transformation * entities["rabbit"]->get_original_vertices()->at(index);

            // La matriz de proyección en perspectiva hace que el último componente del vector
            // transformado no tenga valor 1.0, por lo que hay que normalizarlo dividiendo:

            float divisor = 1.f / vertex.w;

            vertex.x *= divisor;
            vertex.y *= divisor;
            vertex.z *= divisor;
            vertex.w = 1.f;
        }
    }

    void View::render()
    {
        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
        // La coordenada Z se escala a un valor suficientemente grande dentro del
        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).

        mat4 identity(1);
        mat4 scaling = scale(identity, glm::vec3(float(width / 2), float(height / 2), 100000000.f));
        mat4 translation = translate(identity, glm::vec3(float(width / 2), float(height / 2), 0.f));
        mat4 transformation = translation * scaling;

        size_t number_of_vertices = entities["rabbit"]->get_transformed_vertices()->size();

        for (size_t index = 0; index < number_of_vertices; index++)
        {
            entities["rabbit"]->get_display_vertices()->at(index) = ivec4(transformation * entities["rabbit"]->get_transformed_vertices()->at(index));
        }

        // Se borra el framebúffer y se dibujan los triángulos:

        rasterizer.clear();

        int* indices = entities["rabbit"]->get_original_indices()->data();

        // Make const iterator to do this
        for (int *end = indices + entities["rabbit"]->get_original_indices()->size(); indices < end; indices += 3)
        {
            if (is_frontface(entities["rabbit"]->get_transformed_vertices()->data(), indices))
            {
                // Se establece el color del polígono a partir del color de su primer vértice:

                rasterizer.set_color(entities["rabbit"]->get_original_colors()->at(*indices));

                // TODO: Clip vertices

                rasterizer.fill_convex_polygon_z_buffer(entities["rabbit"]->get_display_vertices()->data(), indices, indices + 3);
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