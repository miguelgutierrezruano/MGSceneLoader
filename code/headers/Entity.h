
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#pragma once

#include <vector>
#include <Color_Buffer.hpp>
#include "Transform.h"
#include "Mesh.h"
#include "Light.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace MGVisualizer
{
	class View;

	using argb::Rgb888;
	using  std::vector;

	/// <summary>
	/// Class to render models in the scene
	/// </summary>
	class Entity
	{

		// Define Color as Rgb888
		typedef Rgb888 Color;

	private:

		Entity* parent;

		Transform transform;

		// Mesh vectors foreach mesh of the model
		vector < Mesh > meshes;

	public:

		Entity(const char* model_path, Entity* parent_entity = nullptr);

		Transform* get_transform() { return &transform; }
		Entity* get_parent() { return parent; }

		void update(mat4 projection);
		void render(mat4 transformation, View * view);

	private:

		mat4 get_parent_matrix();

		void load_model_nodes(const char* model_path);

		Color compute_lightning(const Color& vertexColor, const vec4& vertex, const vec4& normal, vector< Light* >& lights);

		void copy_nodes_recursive(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);
		void copy_meshes(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);

		mat4 aiToGlm(const aiMatrix4x4& from);
	};
}