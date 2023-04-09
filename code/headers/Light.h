#pragma once

// Distributed under MIT License
// @miguelgutierrezruano
// 2023

#include <Color.hpp>

namespace MGVisualizer
{

	using argb::Rgb888;

	class Light
	{

		typedef Rgb888 Color;

	protected:

		Color color;
		float intensity;

	public:

		Color get_color    () { return color; }
		float get_intensity() { return intensity; }

		void set_color(Color newColor) { color = newColor; }
		void set_intensity(float newIntensity) { intensity = newIntensity; }

	public:

		Light()
		{
			color = { 1.f, 1.f, 1.f };
			intensity = 1;
		}
	};
}


