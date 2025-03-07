#pragma once
#include <SFML/Window.hpp>

struct Cell
{
	Cell(sf::Vector2f vel_, float density_);
	Cell(float x_vel_, float y_vel_, float density_);
	Cell();

	float x_vel_0;
	float y_vel_0;
	float x_vel_1;
	float y_vel_1;
	float density_0;
	float density_1;
	float divergence;
	float curl;
};
