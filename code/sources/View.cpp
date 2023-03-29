
// Este c�digo es de dominio p�blico.
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
    View::View(unsigned width, unsigned height)
        :
        width(width),
        height(height),
        color_buffer(width, height),
        rasterizer(color_buffer)
    {
        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            mesh_file_path,
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
        );

        // If scene is null file could not be loaded

        if (scene && scene->mNumMeshes > 0)
        {
            // Make loop to get all meshes

            auto mesh = scene->mMeshes[0];

            size_t number_of_vertices = mesh->mNumVertices;

            // Copy vertex coordinates

            original_vertices.resize(number_of_vertices);

            for (size_t index = 0; index < number_of_vertices; index++)
            {
                auto& vertex = mesh->mVertices[index];

                original_vertices[index] = vec4(vertex.x, -vertex.y, vertex.z, 1.f);
            }

            transformed_vertices.resize(number_of_vertices);
            display_vertices.resize(number_of_vertices);

            // Random color to each vertex. TODO: Get color from mesh

            original_colors.resize(number_of_vertices);

            for (size_t index = 0; index < number_of_vertices; index++)
            {
                original_colors[index].set(rand_clamp(), rand_clamp(), rand_clamp());
            }

            // Generate indexes of triangles

            size_t number_of_triangles = mesh->mNumFaces;

            original_indices.resize(number_of_triangles * 3);

            vector<int>::iterator indices_iterator = original_indices.begin();

            for (size_t index = 0; index < number_of_triangles; index++)
            {
                auto& face = mesh->mFaces[index];

                assert(face.mNumIndices == 3);              // Una face puede llegar a tener de 1 a 4 �ndices,
                                                            // pero nos interesa que solo haya tri�ngulos
                auto indices = face.mIndices;

                *indices_iterator++ = int(indices[0]);
                *indices_iterator++ = int(indices[1]);
                *indices_iterator++ = int(indices[2]);
            }
        }
    }

    void View::update()
    {
        // Se actualizan los par�metros de transformati�n (s�lo se modifica el �ngulo):

        static float angle = 0.f;

        angle += 0.025f;

        // Se crean las matrices de transformaci�n:

        mat4 identity(1);
        mat4 scaling = scale(identity, vec3(4, 4, 4));
        mat4 rotation_y = identity;
        mat4 translation = translate(identity, vec3(0.f, 0.5f, -10.f));
        mat4 projection = perspective(20.f, 1.f, 15.f, float(width) / height);

        // Creaci�n de la matriz de transformaci�n unificada:

        mat4 transformation = projection * translation * rotation_y * scaling;

        // Se transforman todos los v�rtices usando la matriz de transformaci�n resultante:

        for (size_t index = 0, number_of_vertices = original_vertices.size(); index < number_of_vertices; index++)
        {
            // Se multiplican todos los v�rtices originales con la matriz de transformaci�n y
            // se guarda el resultado en otro vertex buffer:

            vec4& vertex = transformed_vertices[index] = transformation * original_vertices[index];

            // La matriz de proyecci�n en perspectiva hace que el �ltimo componente del vector
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

        for (size_t index = 0, number_of_vertices = transformed_vertices.size(); index < number_of_vertices; index++)
        {
            // Has to be highp ivec4 
            display_vertices[index] = highp_ivec4(transformation * transformed_vertices[index]);
        }

        // Se borra el frameb�ffer y se dibujan los tri�ngulos:

        rasterizer.clear();

        for (int* indices = original_indices.data(), *end = indices + original_indices.size(); indices < end; indices += 3)
        {
            if (is_frontface(transformed_vertices.data(), indices))
            {
                // Se establece el color del pol�gono a partir del color de su primer v�rtice:

                rasterizer.set_color(original_colors[*indices]);

                // Se rellena el pol�gono:

                rasterizer.fill_convex_polygon_z_buffer(display_vertices.data(), indices, indices + 3);
            }
        }

        // Se copia el frameb�ffer oculto en el frameb�ffer de la ventana:

        color_buffer.blit_to_window();
    }

    bool View::is_frontface(const vec4* const projected_vertices, const int* const indices)
    {
        const vec4& v0 = projected_vertices[indices[0]];
        const vec4& v1 = projected_vertices[indices[1]];
        const vec4& v2 = projected_vertices[indices[2]];

        // Se asumen coordenadas proyectadas y pol�gonos definidos en sentido horario.
        // Se comprueba a qu� lado de la l�nea que pasa por v0 y v1 queda el punto v2:

        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
    }
}