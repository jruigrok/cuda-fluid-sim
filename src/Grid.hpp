#pragma once 
#include <Cell.hpp>
#include <iostream>
#include <math.h>
#include <array>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/Vertex.hpp>

class Grid : public sf::Drawable, sf::Transformable {

public:
	Grid(uint32_t width, uint32_t height, uint32_t cellSize);
	~Grid();
	uint32_t getPos(const uint32_t i, const uint32_t j) const;
	const Cell& getCell(const uint32_t i, const uint32_t j) const;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update();

private:

	void addSource(float Cell::* x_vel, float Cell::* y_vel);
	void initializeVAs();
	void swap(float Cell::*& field, float Cell::*& pastfield);
	void updateVAs();
	void solveDiffusion(float Cell::* field, float Cell::* pastfield, uint32_t p);
	void solveAdvection(float Cell::* field, float Cell::* pastfield, uint32_t p);
	void setBounds(uint32_t b, float Cell::*field);
	void project();
	double totalFluid() const;


	Cell* grid;
    sf::Vertex* cellVertices;
    sf::Vertex* lineVertices;
	const uint32_t width;
	const uint32_t height;
	const uint32_t cellSize;
	static constexpr float diff = 0.00001f;
	static constexpr float dt = 0.0005f;
	static constexpr float viscosity = 1.0;
	sf::VertexBuffer cellVA;
	sf::VertexBuffer lineVA;
	float Cell::* x_vel_buf1 = &Cell::x_vel_1;
	float Cell::* y_vel_buf1 = &Cell::y_vel_1;
	float Cell::* x_vel_buf0 = &Cell::x_vel_0;
	float Cell::* y_vel_buf0 = &Cell::y_vel_0;
	float Cell::* density_buf0 = &Cell::density_0;
	float Cell::* density_buf1 = &Cell::density_1;
};
