
#include "Clipper.h"

namespace MGVisualizer
{
    int Clipper::clip(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices)
    {
        ivec4 aux_vertices[10];

        static int clipped_indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        int n = clip_against_line(vertices, first_index, last_index, aux_vertices, 1.f, 0.f, 0.f);

		for (int* index = clipped_indices; index < clipped_indices + n; index++)
		{
			if (aux_vertices[*index].y < -1000.f)
			{
				int i = 0;
			}
		}

        n = clip_against_line(aux_vertices, clipped_indices, clipped_indices + n, clipped_vertices, 0.f, -1.f, 600.f);
        n = clip_against_line(clipped_vertices, clipped_indices, clipped_indices + n, aux_vertices, -1.f, 0.f, 800.f);
        n = clip_against_line(aux_vertices, clipped_indices, clipped_indices + n, clipped_vertices, 0.f, 1.f, 0.f);
        return n;
    }

    int Clipper::clip_against_line(const ivec4* vertices, int* first_index, int* last_index, ivec4* clipped_vertices, float a, float b, float c)
    {
        int n = 0;
        for (int* index = first_index; index < last_index; index++)
        {
            ivec4 v1 = vertices[*index];

			ivec4 v2;

			if (index < last_index - 1)
			{
				v2 = vertices[*(index + 1)];
			}
			else
			{
				v2 = vertices[*first_index];
			}

            float vertex1_side = a * v1.x + b * v1.y + c;
            float vertex2_side = a * v2.x + b * v2.y + c;

            if (vertex1_side >= 0 && vertex2_side >= 0)
            {
                clipped_vertices[n++] = v2;
                continue;
            }
            else if (vertex1_side < 0 && vertex2_side < 0)
            {

            }
            else if (vertex1_side < 0)
            {
                ivec4 intersection_point = line_intersection(a, b, c, v1, v2);
                clipped_vertices[n++] = intersection_point;
                clipped_vertices[n++] = v2;
                continue;
            }
            else
            {
                ivec4 intersection_point = line_intersection(a, b, c, v1, v2);
                clipped_vertices[n++] = intersection_point;
            }
        }

        return n;
    }

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