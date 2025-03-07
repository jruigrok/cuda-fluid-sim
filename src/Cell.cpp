#include "Cell.hpp"

Cell::Cell(sf::Vector2f vel_, float density_):
    x_vel_0(vel_.x),
    y_vel_0(vel_.y),
    x_vel_1(x_vel_0),
    y_vel_1(y_vel_0),
    divergence(0.0f),
    curl(0.0f),
    density_0(density_),
    density_1(density_)
{}

Cell::Cell(float x_vel_, float y_vel_, float density_):
    x_vel_0(x_vel_),
    y_vel_0(y_vel_),
    x_vel_1(x_vel_),
    y_vel_1(y_vel_),
    divergence(0.0f),
    curl(0.0f),
    density_0(density_),
    density_1(density_)
{}

Cell::Cell() :
    x_vel_0(0.0),
    y_vel_0(0.0),
    x_vel_1(x_vel_0),
    y_vel_1(y_vel_0),
    divergence(0.0f),
    curl(0.0f),
    density_0(static_cast<float>(std::rand()) / RAND_MAX),
    density_1(density_0)
{}