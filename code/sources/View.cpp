
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <iostream>
#include <cassert>
#include <cmath>
#include "View.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace glm;

namespace MGVisualizer
{
    // Initialize view attributes
    View::View(unsigned width, unsigned height)
        :
        width(width),
        height(height),
        color_buffer(width, height),
        rasterizer(color_buffer)
    { 
        Entity* japan = new Entity("../binaries/japan.fbx");
        entities.emplace("japan", japan);

        entities["japan"]->get_transform()->set_position(vec3(20.f, 30.f, -140.f));
        entities["japan"]->get_transform()->set_rotation(vec3(180, 270, 0.f));
        entities["japan"]->get_transform()->set_scale(vec3(0.1f, 0.1f, 0.1f));

        Entity* deer = new Entity("../binaries/deer.obj", japan);
        entities.emplace("deer", deer);

        entities["deer"]->get_transform()->set_position(vec3(400.f, 90.f, 360.f));
        entities["deer"]->get_transform()->set_rotation(vec3(0, 0, 0.f));
        entities["deer"]->get_transform()->set_scale(vec3(0.2f, 0.2f, 0.2f));

        Entity* cloud = new Entity("../binaries/Cloud.obj", japan);
        entities.emplace("cloud", cloud);

        entities["cloud"]->get_transform()->set_position(vec3(0.f, 1000.f, 0.f));
        entities["cloud"]->get_transform()->set_rotation(vec3(0, 0, 0.f));
        entities["cloud"]->get_transform()->set_scale(vec3(70.f, 70.f, 70.f));

        Entity* eagle = new Entity("../binaries/eagle.obj", cloud);
        entities.emplace("eagle", eagle);

        entities["eagle"]->get_transform()->set_position(vec3(5.f, 0.f, 0.f));
        entities["eagle"]->get_transform()->set_rotation(vec3(0, 0, 0.f));
        entities["eagle"]->get_transform()->set_scale(vec3(0.1f, 0.1f, 0.1f));

        /*Entity* triangle = new Entity("../binaries/triangle.obj");
        entities.emplace("triangle", triangle);

        entities["triangle"]->get_transform()->set_position(vec3(0.f, 0.f, 10.f));
        entities["triangle"]->get_transform()->set_rotation(vec3(0, 0, 0.f));
        entities["triangle"]->get_transform()->set_scale(vec3(10.f, 10.f, 10.f));*/

        camera.transform.set_position(vec3(120, -25, 0));
        camera.transform.set_rotation(vec3(10, 35, 0));

        Light* ambientLight = new Light();
        ambientLight->set_intensity(0.1f);
        lights.push_back(ambientLight);

        DirectionalLight* dirLight = new DirectionalLight(vec3(1.f, -1.f, 0.f));
        dirLight->set_intensity(1.f);
        lights.push_back(dirLight);

        mouseLastPosition = vec2();

        worldRotation = 0;
        cloudRotation = 0;
    }

    void View::update()
    {
        // Create projection matrix
        cloudRotation -= 0.5f;
        worldRotation += 0.1f;

        entities["japan"]->get_transform()->set_rotation(vec3(vec3(180, 270 + worldRotation, 0.f)));
		entities["cloud"]->get_transform()->set_rotation(vec3(0, cloudRotation, 0.f));

        // Get projection matrix by moving the camera to (0, 0, 0) and the projection matrix
        mat4 inverseCamera = inverse(camera.transform.get_matrix());
        mat4 projection = camera.get_projection_matrix(float(width) / height) * inverseCamera;

        // Update each entity
        for (auto& [name, entity] : entities)
            entity->update(projection);
    }

    void View::render()
    {
        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
        // La coordenada Z se escala a un valor suficientemente grande dentro del
        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).

        mat4 identity(1);
        mat4 scaling = scale(identity, glm::vec3(float(width / 2), float(height / 2), 1000000.f));
        mat4 translation = translate(identity, glm::vec3(float(width / 2), float(height / 2), 0.f));
        mat4 transformation = translation * scaling;

        rasterizer.clear();

        // Render each entity
        for (auto& [name, entity] : entities)
            entity->render(transformation, this);

        // Se copia el framebúffer oculto en el framebúffer de la ventana:

        color_buffer.blit_to_window();
    }

    void View::process_events(Event& sfEvent, float delta)
    {
        vec2 currentMousePosition = vec2(Mouse::getPosition().x, Mouse::getPosition().y);

        vec2 positionDifference = currentMousePosition - mouseLastPosition;

        mouseLastPosition = currentMousePosition;

        if (Mouse::isButtonPressed(Mouse::Button::Left))
        {
            camera.move_camera(delta, positionDifference);
        }
        else if (Mouse::isButtonPressed(Mouse::Button::Middle))
        {
            camera.move_camera(delta, positionDifference);
        }
        else if(Mouse::isButtonPressed(Mouse::Button::Right))
        {
            if (Keyboard::isKeyPressed(Keyboard::W))
                camera.move_camera_front(delta);
            if (Keyboard::isKeyPressed(Keyboard::A))
                camera.move_camera_left(delta);
            if (Keyboard::isKeyPressed(Keyboard::S))
                camera.move_camera_back(delta);
            if (Keyboard::isKeyPressed(Keyboard::D))
                camera.move_camera_right(delta);

            camera.rotate_camera(delta, positionDifference);
        }   

        if (sfEvent.type == Event::MouseWheelScrolled)
        {
            if (sfEvent.mouseWheelScroll.delta > 0)
                camera.move_camera_front(delta);
            else
                camera.move_camera_back(delta);
        }
    }

    void View::set_rasterizer_color(Color color)
    {
        rasterizer.set_color(color);
    }

    void View::rasterizer_fill_polygon(const ivec4* const vertices, const int* const indices_begin, const int* const indices_end)
    {
        if(vertices != nullptr)
            rasterizer.fill_convex_polygon_z_buffer(vertices, indices_begin, indices_end);
    }

    bool View::is_backface(const vec4* const projected_vertices, const int* const indices)
    {
        const vec4& v0 = projected_vertices[indices[0]];
        const vec4& v1 = projected_vertices[indices[1]];
        const vec4& v2 = projected_vertices[indices[2]];

        // Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
        // Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:

        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
    }
}