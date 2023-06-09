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

	public:

		/// <summary>
		/// Types of lights
		/// </summary>
		enum Types
		{
			Ambient, 
			Directional,

			// TODO: Do point light
			Point
		};

	protected:

		Types type;

		Color color;
		float intensity;

	public:

		Types get_type     () { return type; }

		Color get_color    () { return color; }
		float get_intensity() { return intensity; }

		void set_color(Color newColor) { color = newColor; }
		void set_intensity(float newIntensity) { intensity = newIntensity; }

	public:

		Light()
		{
			type = Light::Ambient;

			color = { 1.f, 1.f, 1.f };
			intensity = 1;
		}

	private:

		// Virtual method needed to make casts
		virtual void illuminate() { }
	};
}


