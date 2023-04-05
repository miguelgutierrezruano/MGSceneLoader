
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <SFML/Window.hpp>
#include <chrono>

#include "Rasterizer.h"
#include "View.h"

using namespace sf;
using namespace std::chrono;
using namespace MGVisualizer;

int main()
{
	// Create the window
	constexpr auto window_width = 800u;
	constexpr auto window_height = 600u;

	Window window(VideoMode(window_width, window_height), "MGSceneLoader", Style::Titlebar | Style::Close);
    View   view(window_width, window_height);

	window.setVerticalSyncEnabled(true);

    // Delta time variables
    auto  chrono = high_resolution_clock();
    float delta_time = 1.f / 60;

    // Run the main loop:

    bool exit = false;

    do
    {
        // Get time where frame started
        high_resolution_clock::time_point start = chrono.now();

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) exit = true;

            view.process_events(event, delta_time);
        }

        view.update();

        view.render();

        window.display();

        delta_time = duration<float>(chrono.now() - start).count();

    } while (not exit);

	return 0;
}