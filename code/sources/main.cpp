
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <SFML/Window.hpp>

#include <Color_Buffer.hpp>
#include <cstdlib>
#include <vector>
#include "Rasterizer.h"

using namespace sf;
using namespace MGVisualizer;

using  std::vector;
using argb::Rgb888;
using argb::Color_Buffer;

int main()
{
	// Create the window
	constexpr auto window_width = 800u;
	constexpr auto window_height = 600u;

	Window window(VideoMode(window_width, window_height), "MGSceneLoader", Style::Titlebar | Style::Close);

	window.setVerticalSyncEnabled(true);

	// Get from view class from Mesh Loader
	{
		typedef Rgb888                Color;
		typedef Color_Buffer< Color > Color_Buffer;

		Color_Buffer               color_buffer(window_width, window_height);
		Rasterizer<Color_Buffer>   rasterizer(color_buffer);
	}

    // Run the main loop:

    bool exit = false;

    do
    {
        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) exit = true;
        }

        window.display();

    } while (not exit);

	return 0;
}