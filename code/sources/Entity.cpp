
#include "Entity.h"

namespace MGVisualizer
{
	Entity::Entity(const char* model_path, vec3 position, vec3 rotation, vec3 scale, Entity* parent_entity)
	{
		transform = Transform(position, rotation, scale);
	}
}