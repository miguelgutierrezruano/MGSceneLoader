
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Entity.h"
#include "View.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace MGVisualizer
{
	Entity::Entity(const char* model_path, Entity* parent_entity, vec3 position, vec3 rotation, vec3 scale)
	{
		transform = Transform(position, rotation, scale);
		parent = parent_entity;

		load_model(model_path);
	}

    void Entity::update(mat4 projection)
    {
        // Apply parent and projection transformations
        mat4 parentMatrix = mat4(1);

        // Get parent matrix
        if (parent != nullptr)
        {
            Entity* parentIt = parent;
            parentMatrix *= parentIt->get_transform()->get_matrix();

            while (parentIt != nullptr)
            {
                parentIt = parentIt->get_parent();

                if (parentIt != nullptr)
                    parentMatrix *= parentIt->get_transform()->get_matrix();
            }
        }

        mat4 transformation = projection * parentMatrix * transform.get_matrix();

        size_t number_of_vertices = original_vertices.size();

        // Transform every vertex by transformation matrix
        for (size_t index = 0; index < number_of_vertices; index++)
        {
            // Save transformed vertex in transformed vertices vector
            vec4& vertex = transformed_vertices.at(index) =
                transformation * original_vertices.at(index);

            // Give w value 1 back, normalize it by dividing

            float divisor = 1.f / vertex.w;

            vertex.x *= divisor;
            vertex.y *= divisor;
            vertex.z *= divisor;
            vertex.w = 1.f;
        }
    }

    void Entity::render(mat4 transformation, View* view)
    {
        size_t number_of_vertices = transformed_vertices.size();

        for (size_t index = 0; index < number_of_vertices; index++)
        {
            display_vertices.at(index) =
                ivec4(transformation * transformed_vertices.at(index));
        }

        // Create size pointers
        int* indices = original_indices.data();
        int* end = indices + original_indices.size();

        // Make const iterator to do this
        for (; indices < end; indices += 3)
        {
            if (view->is_frontface(transformed_vertices.data(), indices))
            {
                // Se establece el color del polígono a partir del color de su primer vértice:

                view->set_rasterizer_color(original_colors.at(*indices));

                // TODO: Clip vertices
                
                view->rasterizer_fill_polygon(display_vertices.data(), indices, indices + 3);
            }
        }
    }

    void Entity::load_model(const char* model_path)
    {
        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            model_path,
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

            original_colors.resize(number_of_vertices);
            transformed_vertices.resize(number_of_vertices);
            display_vertices.resize(number_of_vertices);

            // Random color to each vertex. TODO: Get color from mesh

            for (size_t index = 0; index < number_of_vertices; index++)
            {
                original_colors[index].set(rand_clamp(), rand_clamp(), rand_clamp());
                //original_colors[index].set(&mesh->mColors[0].r, mesh->mColors[0].g, mesh->mColors[0].b);
            }

            // Generate indexes of triangles

            size_t number_of_triangles = mesh->mNumFaces;

            original_indices.resize(number_of_triangles * 3);

            vector<int>::iterator indices_iterator = original_indices.begin();

            for (size_t index = 0; index < number_of_triangles; index++)
            {
                auto& face = mesh->mFaces[index];

                assert(face.mNumIndices == 3);              // Una face puede llegar a tener de 1 a 4 índices,
                                                            // pero nos interesa que solo haya triángulos
                auto indices = face.mIndices;

                *indices_iterator++ = int(indices[0]);
                *indices_iterator++ = int(indices[1]);
                *indices_iterator++ = int(indices[2]);
            }
        }
    }
}