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
	uint32_t getPos(const uint32_t i, const uint32_t j) const;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update();

private:

	void addSource(float Cell::* x_vel, float Cell::* y_vel, float Cell::* density);
	void initializeVAs();
	void updateVAs(float Cell::* x_vel, float Cell::* y_vel, float Cell::* density);
	//void solveDiffusion(float Cell::* field, float Cell::* pastfield);
	void solveAdvection(float Cell::* x_vel_new, float Cell::* y_vel_new, 
						float Cell::* density_new, 
						float Cell::* x_vel, float Cell::* y_vel, 
						float Cell::* density);

	const float sample(const float x, const float y, float Cell::* field, const float dx, const float dy) const;
	void project(float Cell::* x_vel_cur, float Cell::* y_vel_cur);
	double totalFluid() const;


	std::vector<Cell> grid;
	std::vector<sf::Vertex> cellVertices;
	std::vector<sf::Vertex> lineVertices;
	const uint32_t width;
	const uint32_t height;
	const uint32_t cellSize;
	static constexpr float diff = 0.00001f;
	static constexpr float dt = 0.15f;
	static constexpr float viscosity = 1.0f;
	static constexpr float over_relaxation = 1.9f;
	static constexpr uint32_t num_iter = 20u;
	sf::VertexBuffer cellVA;
	sf::VertexBuffer lineVA;
	float Cell::* x_vel_buf1 = &Cell::x_vel_1;
	float Cell::* y_vel_buf1 = &Cell::y_vel_1;
	float Cell::* x_vel_buf0 = &Cell::x_vel_0;
	float Cell::* y_vel_buf0 = &Cell::y_vel_0;
	float Cell::* density_buf0 = &Cell::density_0;
	float Cell::* density_buf1 = &Cell::density_1;
};
