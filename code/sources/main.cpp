
// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <SFML/Window.hpp>

using namespace sf;

int main()
{
	// Create the window
	constexpr auto window_width = 800u;
	constexpr auto window_height = 600u;

	Window window(VideoMode(window_width, window_height), "MGSceneLoader", Style::Titlebar | Style::Close);

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

        window.display();

    } while (not exit);

	return 0;
}