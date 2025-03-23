#include "Cell.hpp"

Cell::Cell(sf::Vector2f vel_, float density_, Cell_Type cell_type_):
    x_vel_0(vel_.x),
    y_vel_0(vel_.y),
    x_vel_1(x_vel_0),
    y_vel_1(y_vel_0),
    density_0(density_),
    density_1(density_),
    cell_type(cell_type_)
{}

Cell::Cell(float x_vel_, float y_vel_, float density_, Cell_Type cell_type_):
    x_vel_0(x_vel_),
    y_vel_0(y_vel_),
    x_vel_1(x_vel_),
    y_vel_1(y_vel_),
    density_0(density_),
    density_1(density_),
    cell_type(cell_type_)
{}

Cell::Cell() :
    x_vel_0(0.0),
    y_vel_0(0.0),
    x_vel_1(x_vel_0),
    y_vel_1(y_vel_0),
    density_0(0),
    density_1(density_0)
{}

Cell::Cell(Cell_Type cell_type_) :
    x_vel_0(0.0),
    y_vel_0(0.0),
    x_vel_1(x_vel_0),
    y_vel_1(y_vel_0),
    density_0(0),
    density_1(density_0),
    cell_type(cell_type_)
{}