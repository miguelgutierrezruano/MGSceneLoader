
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Entity.h"
#include "View.h"
#include "Clipper.h"

namespace MGVisualizer
{
	Entity::Entity(const char* model_path, Entity* parent_entity, vec3 position, vec3 rotation, vec3 scale)
	{
		transform = Transform(position, rotation, scale);
		parent = parent_entity;

		load_model_nodes(model_path);
	}

    void Entity::load_model_nodes(const char* model_path)
    {
        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            model_path,
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals
        );

        if (scene && scene->mNumMeshes > 0)
        {
            // For now only root node, iterate each node recursively
            aiNode* root = scene->mRootNode;

            copy_nodes_recursive(root, scene, root->mTransformation);
        }
    }

    void Entity::copy_nodes_recursive(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform)
    {
        // If node has meshes copy them
        if (node->mNumMeshes > 0)
        {
            copy_meshes(node, scene, parentTransform);
        }

        for (unsigned i = 0; i < node->mNumChildren; i++)
        {
            copy_nodes_recursive(node->mChildren[i], scene, parentTransform * node->mTransformation);
        }
    }

    void Entity::copy_meshes(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform)
    {
        for (unsigned i = 0; i < node->mNumMeshes; i++)
        {
            // Make a MGMesh for each mesh in node
            Mesh mgMesh;

            // Get mesh
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            // Get number of vertices and resize proper vectors
            size_t vertices_number = mesh->mNumVertices;

            mgMesh.original_vertices.resize(vertices_number);
            mgMesh.original_colors.resize(vertices_number);
            mgMesh.original_normals.resize(vertices_number);
            mgMesh.transformed_vertices.resize(vertices_number);
            mgMesh.transformed_normals.resize(vertices_number);
            mgMesh.display_vertices.resize(vertices_number);
            mgMesh.computed_colors.resize(vertices_number);

            // Get number of triangles and resize proper vectors
            size_t triangles_number = mesh->mNumFaces;

            mgMesh.original_indices.resize(triangles_number * 3);

            // Get color of mesh
            aiColor4D diffuse_color;

            // Get material of mesh
            auto material = scene->mMaterials[mesh->mMaterialIndex];
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color);

            // Calculate transformation
            mat4 transformation = aiToGlm(parentTransform) * aiToGlm(node->mTransformation);

            // Iterate every vertex in mesh
            for (size_t index = 0; index < mesh->mNumVertices; index++)
            {
                // Copy vertex coordinates
                auto& vertex = mesh->mVertices[index];
                mgMesh.original_vertices[index] = transformation * vec4(vertex.x, vertex.y, vertex.z, 1.f);

                // Copy color coordinates
                mgMesh.original_colors[index].set(diffuse_color.r, diffuse_color.g, diffuse_color.b);

                auto& normal = mesh->mNormals[index];
                mgMesh.original_normals[index] = transformation * vec4(normal.x, normal.y, normal.z, 0.f);
            }

            auto indices_iterator = mgMesh.original_indices.begin();

            // Generate indexes of triangles
            for (size_t index = 0; index < mesh->mNumFaces; index++)
            {
                auto& face = mesh->mFaces[index];

                // Make sure mesh is properly triangulated
                assert(face.mNumIndices == 3);

                auto indices = face.mIndices;

                *indices_iterator++ = (int(indices[0]));
                *indices_iterator++ = (int(indices[1]));
                *indices_iterator++ = (int(indices[2]));
            }

            meshes.push_back(mgMesh);
        }
    }

    void Entity::update(mat4 projection)
    {
        // Apply parent and projection transformations
        mat4 parentMatrix = get_parent_matrix();

        mat4 transformation = projection * parentMatrix * transform.get_matrix();

        size_t meshes_number = meshes.size();

        // Iterate all meshes
        for (int i = 0; i < meshes_number; i++)
        {
            Mesh* mesh = &meshes[i];

            size_t number_of_vertices = mesh->original_vertices.size();

            // Transform every vertex by transformation matrix
            for (size_t index = 0; index < number_of_vertices; index++)
            {
                // Save transformed vertex in transformed vertices vector
                vec4& vertex = mesh->transformed_vertices[index] =
                    transformation * mesh->original_vertices[index];

                // Since we only need world normals we dont multiply projection
                vec4& normal = mesh->transformed_normals[index] =
                    parentMatrix * transform.get_matrix() * mesh->original_normals[index];

                // Normalize vertex
                float divisor = 1.f / vertex.w;

                vertex.x *= divisor;
                vertex.y *= divisor;
                vertex.z *= divisor;
                vertex.w = 1.f;

                // Normalize normal
                vec3 normalizedNormal = normalize(vec3(normal.x, normal.y, normal.z));
                normal = vec4(normalizedNormal.x, normalizedNormal.y, normalizedNormal.z, 0.f);
            }
        }
    }

    // Give vector of lights
    void Entity::render(mat4 transformation, View* view)
    {
        size_t meshes_number = meshes.size();

        // Iterate all meshes
        for (int i = 0; i < meshes_number; i++)
        {
            Mesh* mesh = &meshes[i];

            size_t number_of_vertices = mesh->transformed_vertices.size();

            // Transform every vertex of the mesh to view 
            for (size_t index = 0; index < number_of_vertices; index++)
            {
                mesh->display_vertices[index] =
                    ivec4(transformation * mesh->transformed_vertices[index]);

				// Compute lightning needs: Vertex world position, light vector, normal world position, vertex color
				mesh->computed_colors[index] = compute_lightning(mesh->original_colors[index],
					get_parent_matrix() * transform.get_matrix() * mesh->original_vertices[index], // World vertex
					mesh->transformed_normals[index], // World normals
					view->get_lights());
            }

            // Create size pointers
            int* indices = mesh->original_indices.data();
            int* end = indices + mesh->original_indices.size();

            vector< int > clip_indices;
            const float inverse255 = 1.f / 255.f;

            for (; indices < end; indices += 3)
            {
                if (view->is_frontface(mesh->transformed_vertices.data(), indices))
                {
                    // Set color with the mean of the three vertexes
                    vec3 polygonColor = vec3(0, 0, 0);

                    bool inside = true;

                    for (auto index = indices; index < indices + 3; index++)
                    {
                        // When converted to float colors go random
                        polygonColor += vec3(mesh->computed_colors[*index].red() * inverse255,
                            mesh->computed_colors[*index].green() * inverse255,
                            mesh->computed_colors[*index].blue() * inverse255);

                        // Clip vertices
                        if (mesh->display_vertices[*index].x > (int)view->width ||
                            mesh->display_vertices[*index].x < 0 ||
                            mesh->display_vertices[*index].y > (int)view->height ||
                            mesh->display_vertices[*index].y < 0)
                            inside = false;
                    }

                    polygonColor = vec3(polygonColor.r / 3, polygonColor.g / 3, polygonColor.b / 3);

                    view->set_rasterizer_color(Color(polygonColor.r, polygonColor.g, polygonColor.b));

                    if(inside)
                        view->rasterizer_fill_polygon(mesh->display_vertices.data(), indices, indices + 3);
                    else
                    {
                        ivec4 clipped_vertices[10];
                        const static int clipped_indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

                        int n = Clipper::clip(mesh->display_vertices.data(), indices, indices + 3, clipped_vertices);

                        // If clipped vertices make a polygon then fill it
                        if(n > 2)
                            view->rasterizer_fill_polygon(clipped_vertices, clipped_indices, clipped_indices + n);                         
                    }
                }
            }
        }
    }

    mat4 Entity::get_parent_matrix()
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

        return parentMatrix;
    }

    Entity::Color Entity::compute_lightning(const Color& vertexColor, const vec4& vertex, const vec4& normal, vector<Light*>& lights)
    {
        const float inverse255 = 1.f / 255.f;

        vec3 result = vec3(0, 0, 0);

        for (size_t i = 0; i < lights.size(); i++)
        {
            switch (lights[i]->get_type())
            {
                case Light::Ambient:
                {
                    // Get ambient light
                    float ambientIntensity = lights[i]->get_intensity();
                    Color ambientColor = lights[i]->get_color();

                    vec3 ambientLight = vec3(ambientColor.red() * ambientIntensity * inverse255,
                        ambientColor.green() * ambientIntensity * inverse255,
                        ambientColor.blue() * ambientIntensity * inverse255);

                    result += vec3(ambientLight.r,
                        ambientLight.g,
                        ambientLight.b);
                }
                break;

                case Light::Directional:
                {
                    // Get light
                    DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lights[i]);

                    float dirIntensity = dirLight->get_intensity();
                    Color dirColor = dirLight->get_color();

                    vec3 directionalLightColor = vec3(dirColor.red() * dirIntensity * inverse255,
                        dirColor.green() * dirIntensity * inverse255,
                        dirColor.blue() * dirIntensity * inverse255);

					// Clamp this
                    float diff = dot(vec3(normal.x, normal.y, normal.z), dirLight->get_direction());

                    diff = diff < 0 ? 0 : diff;

                    vec3 diffuse = diff * directionalLightColor;

                    result += vec3(diffuse.r,
                        diffuse.g,
                        diffuse.b);
                }
                break;

                default:
                    break;
            }
        }

        return Color(result.r * vertexColor.red() * inverse255, result.g * vertexColor.green() * inverse255, result.b * vertexColor.blue() * inverse255);
    }

    mat4 Entity::aiToGlm(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
        to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
        to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
        to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;
        return to;
    }
}