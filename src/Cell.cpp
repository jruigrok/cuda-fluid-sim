#include "Cell.hpp"

Cell::Cell(sf::Vector2f vel_, Cell_Type cell_type_):
    x_vel(vel_.x),
    y_vel(vel_.y),
    p_x_vel(0.0),
    p_y_vel(0.0),
    cell_type(cell_type_),
    s(cell_type_ != Cell_Type::SOLID),
    density(0.0f)
{}

Cell::Cell(float x_vel_, float y_vel_, Cell_Type cell_type_):
    x_vel(x_vel_),
    y_vel(y_vel_),
    p_x_vel(x_vel_),
    p_y_vel(y_vel_),
    cell_type(cell_type_),
    s(cell_type_ != Cell_Type::SOLID),
    density(0.0f)
{}

Cell::Cell() :
    x_vel(0.0),
    y_vel(0.0),
    p_x_vel(0.0),
    p_y_vel(0.0),
    cell_type(Cell_Type::LIQUID),
    s(cell_type != Cell_Type::SOLID),
    density(0.0f)
{}

Cell::Cell(Cell_Type cell_type_) :
    x_vel(0.0),
    y_vel(0.0),
    p_x_vel(0.0),
    p_y_vel(0.0),
    cell_type(cell_type_),
    s(cell_type_ != Cell_Type::SOLID),
    density(0.0f)
{}