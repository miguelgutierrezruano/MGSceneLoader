
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include "Entity.h"
#include "View.h"

namespace MGVisualizer
{
	Entity::Entity(const char* model_path, Entity* parent_entity, vec3 position, vec3 rotation, vec3 scale)
	{
		transform = Transform(position, rotation, scale);
		parent = parent_entity;

		load_model_nodes(model_path);
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
                vec4& vertex = mesh->transformed_vertices.at(index) =
                    transformation * mesh->original_vertices.at(index);

                // Give w value 1 back, normalize it by dividing

                float divisor = 1.f / vertex.w;

                vertex.x *= divisor;
                vertex.y *= divisor;
                vertex.z *= divisor;
                vertex.w = 1.f;
            }
        }
    }

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
                mesh->display_vertices.at(index) =
                    ivec4(transformation * mesh->transformed_vertices.at(index));
            }

            // Create size pointers
            int* indices = mesh->original_indices.data();
            int* end = indices + mesh->original_indices.size();

            vector< int > clip_indices;

            for (; indices < end; indices += 3)
            {
                if (view->is_frontface(mesh->transformed_vertices.data(), indices))
                {
                    // Se establece el color del polígono a partir del color de su primer vértice:

                    view->set_rasterizer_color(mesh->original_colors.at(*indices));

                    bool inside = true;

                    // Clip vertices
                    for (auto index = indices; index < indices + 3; index++)
                    {
                        if (mesh->display_vertices.at(*index).x > (int)view->width ||
                            mesh->display_vertices.at(*index).x < 0 ||
                            mesh->display_vertices.at(*index).y > (int)view->height ||
                            mesh->display_vertices.at(*index).y < 0)
                            inside = false;
                    }

                    if(inside)
                        view->rasterizer_fill_polygon(mesh->display_vertices.data(), indices, indices + 3);
                    //else
                    //{
                    //    auto clipped_vertices = clip_triangle(mesh->display_vertices.data(), indices, view->width, view->height);

                    //    if (clipped_vertices.size() > 0)
                    //    {
                    //        clip_indices.clear();
                    //        clip_indices.reserve(clipped_vertices.size());

                    //        for (int i = 0; i < (int)clipped_vertices.size(); i++)
                    //            clip_indices.push_back(i);

                    //        const int* clip_start = clip_indices.data();
                    //        const int* clip_end = clip_start + clip_indices.size();

                    //        // Commented until doubts are resolved
                    //        //view->rasterizer_fill_polygon(clipped_vertices.data(), clip_start, clip_end);
                    //    }
                    //}
                }
            }
        }
    }

    void Entity::load_model_nodes(const char* model_path)
    {
        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            model_path,
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType // Generate normals with assimp
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

        for (int i = 0; i < node->mNumChildren; i++)
        {
            copy_nodes_recursive(node->mChildren[i], scene, parentTransform * node->mTransformation);
        }
    }

    void Entity::copy_meshes(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform)
    {
        // Make a MGMesh for each mesh in node
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            Mesh mgMesh;
            // Get mesh
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            // Get number of vertices and resize proper vectors
            size_t vertices_number = mesh->mNumVertices;

            mgMesh.original_vertices.resize(vertices_number);
            mgMesh.original_colors.resize(vertices_number);
            mgMesh.transformed_vertices.resize(vertices_number);
            mgMesh.display_vertices.resize(vertices_number);

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

    // Apply Sutherland-Hodgman algorithm
    vector< ivec4 > Entity::clip_triangle(const ivec4* const vertices, const int* const indices_begin, int width, int height)
    {
        // Create array with the given triangle
        vector< ivec4 > output = { vertices[*indices_begin], vertices[*indices_begin + 1], vertices[*indices_begin + 2] };

        // Clip left side
        output = clip_left(output, 0);
        // Clip bottom side
        if (output.size() > 0)
            output = clip_top(output, 0);
        // Clip right side
        if (output.size() > 0)
            output = clip_right(output, width);
        // Clip top side
        if (output.size() > 0)
            output = clip_bottom(output, height);

        return output;
    }

    ivec4 Entity::get_intersection_horizontal(ivec4 point1, ivec4 point2, int yIntersect)
    {
        int xDiff = abs(point2.x - point1.x);
        int yDiff = abs(point2.y - point1.y);

        // If line is horizontal then return point
        if (yDiff == 0) return point1;

        int t = (yIntersect - point1.y) / yDiff;
        int x = point1.x + t * xDiff;

        return ivec4(x, yIntersect, 1, 1);
    }

    ivec4 Entity::get_intersection_vertical(ivec4 point1, ivec4 point2, int xIntersect)
    {
        int xDiff = abs(point2.x - point1.x);
        int yDiff = abs(point2.y - point1.y);

        // If line is vertical then return point
        if (xDiff == 0) return point1;

        int t = (xIntersect - point1.x) / xDiff;
        int y = point1.y + t * yDiff;

        return ivec4(xIntersect, y, 1, 1);
    }

    vector<ivec4> Entity::clip_left(const vector<ivec4>& polygon, int xMin)
    {
        vector< ivec4 > result;

        size_t numVertices = polygon.size();
        ivec4 currentVertex = polygon[numVertices - 1];

        for (size_t i = 0; i < numVertices; i++)
        {
            ivec4 comparedVertex = polygon[i];

            // Compared vertex is inside of the clippinng window
            if (comparedVertex.x >= xMin)
            {
                // Current vertex is outside push intersection
                if (currentVertex.x < xMin)
                {
                    ivec4 intersect = get_intersection_vertical(currentVertex, comparedVertex, xMin);
                    result.push_back(intersect);
                }

                // Always push compared
                result.push_back(comparedVertex);
            }
            // If compared is outside and current is inside push intersection
            else if (currentVertex.x >= xMin)
            {
                ivec4 intersect = get_intersection_vertical(currentVertex, comparedVertex, xMin);
                result.push_back(intersect);
            }
            // If both are out they get discarded

            currentVertex = comparedVertex;
        }

        return result;
    }

    vector<ivec4> Entity::clip_right(const vector<ivec4>& polygon, int xMax)
    {
        vector< ivec4 > result;

        size_t numVertices = polygon.size();
        ivec4 currentVertex = polygon[numVertices - 1];

        for (size_t i = 0; i < numVertices; i++)
        {
            ivec4 comparedVertex = polygon[i];

            // Compared vertex is inside of the clippinng window
            if (comparedVertex.x <= xMax)
            {
                // Current vertex is outside push intersection
                if (currentVertex.x > xMax)
                {
                    ivec4 intersect = get_intersection_vertical(currentVertex, comparedVertex, xMax);
                    result.push_back(intersect);
                }

                // Always push compared
                result.push_back(comparedVertex);
            }
            // If compared is outside and current is inside push intersection
            else if (currentVertex.x <= xMax)
            {
                ivec4 intersect = get_intersection_vertical(currentVertex, comparedVertex, xMax);
                result.push_back(intersect);
            }
            // If both are out they get discarded

            currentVertex = comparedVertex;
        }

        return result;
    }

    vector<ivec4> Entity::clip_top(const vector<ivec4>& polygon, int yMin)
    {
        vector< ivec4 > result;

        size_t numVertices = polygon.size();
        ivec4 currentVertex = polygon[numVertices - 1];

        for (size_t i = 0; i < numVertices; i++)
        {
            ivec4 comparedVertex = polygon[i];

            // Compared vertex is inside of the clippinng window
            if (comparedVertex.y >= yMin)
            {
                // Current vertex is outside push intersection
                if (currentVertex.y < yMin)
                {
                    ivec4 intersect = get_intersection_horizontal(currentVertex, comparedVertex, yMin);
                    result.push_back(intersect);
                }

                // Always push compared
                result.push_back(comparedVertex);
            }
            // If compared is outside and current is inside push intersection
            else if (currentVertex.y >= yMin)
            {
                ivec4 intersect = get_intersection_horizontal(currentVertex, comparedVertex, yMin);
                result.push_back(intersect);
            }
            // If both are out they get discarded

            currentVertex = comparedVertex;
        }

        return result;
    }

    vector<ivec4> Entity::clip_bottom(const vector<ivec4>& polygon, int yMax)
    {
        vector< ivec4 > result;

        size_t numVertices = polygon.size();
        ivec4 currentVertex = polygon[numVertices - 1];

        for (size_t i = 0; i < numVertices; i++)
        {
            ivec4 comparedVertex = polygon[i];

            // Compared vertex is inside of the clippinng window
            if (comparedVertex.y <= yMax)
            {
                // Current vertex is outside push intersection
                if (currentVertex.y > yMax)
                {
                    ivec4 intersect = get_intersection_horizontal(currentVertex, comparedVertex, yMax);
                    result.push_back(intersect);
                }

                // Always push compared
                result.push_back(comparedVertex);
            }
            // If compared is outside and current is inside push intersection
            else if (currentVertex.y <= yMax)
            {
                ivec4 intersect = get_intersection_horizontal(currentVertex, comparedVertex, yMax);
                result.push_back(intersect);
            }
            // If both are out they get discarded

            currentVertex = comparedVertex;
        }

        return result;
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