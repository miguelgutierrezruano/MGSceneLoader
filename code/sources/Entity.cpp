
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Entity.h"

namespace MGVisualizer
{
	Entity::Entity(const char* model_path, vec3 position, vec3 rotation, vec3 scale, Entity* parent_entity)
	{
		transform = Transform(position, rotation, scale);
	}

	// Include assimp, load model vertexes and 
	void Entity::load_model(const char* model_path)
	{

	}
}