#pragma once
#include <SFML/Window.hpp>
#include <array>

enum Cell_Type : uint8_t { LIQUID = 1, SOLID = 0 };

struct Cell
{
	Cell(sf::Vector2f vel_, Cell_Type cell_type_);
	Cell(float x_vel_, float y_vel_, Cell_Type cell_type_);
	Cell();
	Cell(Cell_Type cell_type_);

	float x_vel_0;
	float y_vel_0;
	float x_vel_1;
	float y_vel_1;
	Cell_Type cell_type;
};
