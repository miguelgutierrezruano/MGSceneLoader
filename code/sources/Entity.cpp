
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Entity.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace MGVisualizer
{
	Entity::Entity(const char* model_path, vec3 position, vec3 rotation, vec3 scale, Entity* parent_entity)
	{
		transform = Transform(position, rotation, scale);
		parent = parent_entity;

		load_model(model_path);
	}

	// Include assimp, load model vertexes and 
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