
#include "Clipper.h"

namespace MGVisualizer
{
    int Clipper::clip(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices, int width, int height)
    {
        // Auxiliar array to keep vertices in each plane
        ivec4 aux_vertices[10];
        int clipped_indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // Clip polygon with left border
        int n = clip_against_line(vertices, first_index, last_index, aux_vertices, 1.f, 0.f, 0.f);

        // Clip polygon with bottom border
        n = clip_against_line(aux_vertices, clipped_indices, clipped_indices + n, clipped_vertices, 0.f, -1.f, (float)height);

        // Clip polygon with right border
        n = clip_against_line(clipped_vertices, clipped_indices, clipped_indices + n, aux_vertices, -1.f, 0.f, float(width));

        // Clip polygon with top border
        n = clip_against_line(aux_vertices, clipped_indices, clipped_indices + n, clipped_vertices, 0.f, 1.f, 0.f);

        return n;
    }

    int Clipper::clip_against_line(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices, float a, float b, float c)
    {
        int n = 0;

        // Iterate polygon vertices
        for (int* index = first_index; index < last_index; index++)
        {
            ivec4 v1 = vertices[*index];

            // If next index is the last one v2 is first
			ivec4 v2 = vertices[index < last_index - 1 ? *(index + 1) : *first_index];

            float vertex1_side = a * v1.x + b * v1.y + c;
            float vertex2_side = a * v2.x + b * v2.y + c;

            // Both vertices are inside 
            if (vertex1_side >= 0 && vertex2_side >= 0)
            {
                clipped_vertices[n++] = v2;
                continue;
            }
            // Both are outside
            else if (vertex1_side < 0 && vertex2_side < 0)
            {

            }
            // First is outside and second is inside
            else if (vertex1_side < 0)
            {
                ivec4 intersection_point = line_intersection(a, b, c, v1, v2);
                clipped_vertices[n++] = intersection_point;
                clipped_vertices[n++] = v2;
                continue;
            }
            // First is inside and second its outside
            else
            {
                ivec4 intersection_point = line_intersection(a, b, c, v1, v2);
                clipped_vertices[n++] = intersection_point;
            }
        }

        return n;
    }

    // Calculate intersection of two lines given by general ecuation and two points
    ivec4 Clipper::line_intersection(float a, float b, float c, const ivec4& point0, const ivec4& point1)
    {
		float divisor = (a * (point1.x - point0.x) + b * (point1.y - point0.y));

        float t = (-a * point0.x - b * point0.y - c) / divisor;

        float x = point0.x + (point1.x - point0.x) * t;
        float y = point0.y + (point1.y - point0.y) * t;
        float z = point0.z + (point1.z - point0.z) * t;

        return ivec4(x, y, z, 1.f);
    }
}