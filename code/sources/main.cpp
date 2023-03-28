
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <SFML/Window.hpp>

#include "Rasterizer.h"
#include "View.h"

using namespace sf;
using namespace MGVisualizer;

int main()
{
	// Create the window
	constexpr auto window_width = 800u;
	constexpr auto window_height = 600u;

	Window window(VideoMode(window_width, window_height), "MGSceneLoader", Style::Titlebar | Style::Close);
    View   view(window_width, window_height);

	window.setVerticalSyncEnabled(true);

    // Run the main loop:

    bool exit = false;

    do
    {
        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) exit = true;
        }

        view.update();

        view.render();

        window.display();

    } while (not exit);

	return 0;
}