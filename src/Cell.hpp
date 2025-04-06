#pragma once
#include <SFML/Window.hpp>
#include <array>

enum Cell_Type : uint8_t { LIQUID = 2, SOLID = 0, AIR = 1 };

struct Cell
{
	Cell(sf::Vector2f vel_, Cell_Type cell_type_);
	Cell(float x_vel_, float y_vel_, Cell_Type cell_type_);
	Cell();
	Cell(Cell_Type cell_type_);

	void setCellType(Cell_Type new_type) {
		cell_type = new_type;
		s = new_type != Cell_Type::SOLID;
	}
	uint8_t getS() { return s; }
    Cell_Type getCellType() { return cell_type; }

	float x_vel;
	float y_vel;
	float p_x_vel;
	float p_y_vel;
	float density;

private:
	Cell_Type cell_type;
	uint8_t s;
};
