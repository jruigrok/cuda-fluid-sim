#pragma once 
#include <Cell.hpp>
#include <iostream>
#include <math.h>
#include <array>
#include <random>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>

struct Particle {
	float x_pos;
	float y_pos;
	float x_vel;
	float y_vel;
};

class Grid : public sf::Drawable, sf::Transformable {

public:
	enum Field : uint8_t{
		X_VEL,
		Y_VEL
	};
	Grid(const uint32_t width, const uint32_t height, const uint32_t cell_size, const uint32_t num_particles);
	const uint32_t getPos(const uint32_t x, const uint32_t y) const;
	const Cell& getCell(const uint32_t x, const uint32_t y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void setField(const Field field, const float value, const uint32_t x, const uint32_t y);
	void setCellType(const Cell_Type cell_type, const uint32_t x, const uint32_t y);
	void update();	

private:

	void updateVAs(float Cell::* x_vel, float Cell::* y_vel);
	const float sample(const float x, const float y, float Cell::* field, const float dx, const float dy) const;
	void project(float Cell::* x_vel_cur, float Cell::* y_vel_cur);
	void addParticlesToMap();
	void collide(const uint32_t pos1, const uint32_t pos2);
	void updateParticles();
	static float genRndFloat(const float min, const float max);

	const uint32_t cell_width;
	const uint32_t cell_height;
	const uint32_t cell_size;
	const uint32_t num_cells;
	const uint32_t num_particles;
	const uint32_t width;
	const uint32_t height;
	const uint32_t cell_size_sqrd;
	const float radius;
	static constexpr float DIFF = 0.00001f;
	static constexpr float DT = 0.15f;
	static constexpr float VISCOSITY = 1.0f;
	static constexpr float OVER_RELAXATION = 1.9f;
	static constexpr float PADDING = 0.01f;
	static constexpr uint32_t NUM_ITER = 20u;
	static constexpr uint32_t IMG_SIZE = 1024;
	static const std::string_view CIRCLE_PNG_FILEPATH;
	float Cell::* x_vel_buf1 = &Cell::x_vel_1;
	float Cell::* y_vel_buf1 = &Cell::y_vel_1;
	float Cell::* x_vel_buf0 = &Cell::x_vel_0;
	float Cell::* y_vel_buf0 = &Cell::y_vel_0;
	std::vector<Cell> grid;
	std::vector<sf::Vertex> cell_vertices;
	std::vector<sf::Vertex> particle_vertices;
	std::vector<sf::Vertex> line_vertices;
	sf::VertexBuffer cell_VA;
	sf::VertexBuffer particle_VA;
	sf::VertexBuffer line_VA;
	std::vector<std::array<Particle*, 4>> particle_map;
	std::vector<uint8_t> particle_map_size;
	std::vector<Particle> particles;
	sf::Texture circle_img;
};

