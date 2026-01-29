#pragma once
#include <Cell.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <array>
#include <iostream>
#include <math.h>
#include <optional>
#include <random>

struct Particle {
  float x_pos;
  float y_pos;
  float x_vel;
  float y_vel;
};

class Grid : public sf::Drawable, sf::Transformable {

public:
  Grid(const uint32_t width, const uint32_t height, const uint32_t f_cell_ratio,
       const uint32_t num_particles, const uint32_t scale);
  const uint32_t getFPos(const uint32_t x, const uint32_t y) const;
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
  void setCellField(float Cell::*field, const float value, const uint32_t x,
                    const uint32_t y);
  void setCellType(const Cell_Type cell_type, const uint32_t x,
                   const uint32_t y);
  void update();

private:
  struct cellData {
    const std::array<uint32_t, 4> f_pos;
    const std::array<float, 4> diff;
  };

  void updateVAs();
  void project();
  void addParticlesToMap();
  void updateParticles();
  void particleVelToCell();
  void cellVelToParticle();
  void updateDensity();
  void collide(const uint32_t pos1, const uint32_t pos2);
  void collideSolid(const uint32_t pos1, const uint32_t pos2);
  void copyField(float Cell::*from, float Cell::*to);
  void pBounds(Particle &p);
  const uint32_t getPPos(const uint32_t x, const uint32_t y) const;
  const uint32_t P2F(const uint32_t x, const uint32_t y) const;
  const uint32_t P2F(const uint32_t p_pos) const;
  const std::array<bool, 4> getValidArr(const std::array<uint32_t, 4> f_pos,
                                        const uint32_t offset);
  const float getPic(float Cell::*feild, const std::array<uint32_t, 4> f_pos,
                     const std::array<float, 4> vd, const float vd_s);
  const float getFlip(float Cell::*feild, float Cell::*pre_feild,
                      const std::array<uint32_t, 4> f_pos,
                      const std::array<float, 4> vd, const float vd_s);
  const cellData getData(Particle &p, const bool x, const bool y);
  static const float genRndFloat(const float min, const float max);

  static constexpr float GRAVITY = 50.0f;
  static constexpr float DT = (1.0 / 165.0) * 10;
  static constexpr float OVER_RELAXATION = 1.9f;
  static constexpr float PADDING = 0.01f;
  static constexpr float FLIP = 0.9f;
  static constexpr uint32_t DIAMETER = 1;
  static constexpr uint32_t DIAMETER_SQRD = DIAMETER * DIAMETER;
  static constexpr float RADIUS = DIAMETER / 2.0f;
  static constexpr float RADIUS_SQRD = RADIUS * RADIUS;
  static constexpr uint32_t IMG_SIZE = 1024;
  static constexpr uint8_t P_CELL_ARR_SIZE = 4;
  static constexpr uint8_t K = 1;

  static constexpr uint32_t NUM_P_SUB_STEP = 1u;
  const uint32_t p_width;
  const uint32_t p_height;
  const uint32_t num_p_cells;
  const uint32_t num_particles;
  float rest_density;
  std::vector<sf::Vertex> particle_vertices;
  sf::VertexBuffer particle_VA;
  std::vector<Particle> particles;
  std::vector<std::array<Particle *, P_CELL_ARR_SIZE>> p_grid;
  std::vector<uint8_t> p_grid_size;
  std::vector<float> r_x;
  std::vector<float> r_y;

  static constexpr uint32_t NUM_F_SUB_STEP = 8u;
  const uint32_t f_width;
  const uint32_t f_height;
  const uint32_t num_f_cells;
  const uint32_t f_cell_size;
  std::vector<sf::Vertex> f_cell_vertices;
  sf::VertexBuffer f_cell_VA;
  std::vector<sf::Vertex> line_vertices;
  sf::VertexBuffer line_VA;
  std::vector<Cell> f_grid;

  const uint32_t render_scale;
  sf::Texture circle_img;
  static const std::string_view CIRCLE_PNG_FILEPATH;
};
