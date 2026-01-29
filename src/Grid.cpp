#include "Grid.hpp"
#include <assert.h>

const std::string_view Grid::CIRCLE_PNG_FILEPATH = CIRCLE_IMG_PATH;

Grid::Grid(const uint32_t width, const uint32_t height,
           const uint32_t f_cell_ratio_, const uint32_t num_particles_,
           const uint32_t render_scale_)
    : f_width(width), f_height(height), f_cell_size(f_cell_ratio_),
      num_f_cells(f_width * f_height), num_particles(num_particles_),
      render_scale(render_scale_), p_width(width * f_cell_ratio_),
      p_height(height * f_cell_ratio_), num_p_cells(p_width * p_height) {
  if (!circle_img.loadFromFile(CIRCLE_PNG_FILEPATH.data())) {
    throw std::runtime_error("Error file not found " +
                             *CIRCLE_PNG_FILEPATH.data());
  }
  circle_img.setSmooth(true);
  circle_img.generateMipmap();

  f_grid.resize(num_f_cells);
  r_x.resize(num_f_cells);
  r_y.resize(num_f_cells);
  f_cell_vertices.resize(num_f_cells * 4);
  line_vertices.resize(num_f_cells * 4);
  f_cell_VA.create(num_f_cells * 4);
  line_VA.create(num_f_cells * 4);
  f_cell_VA.setPrimitiveType(sf::Quads);
  line_VA.setPrimitiveType(sf::Lines);

  p_grid.resize(num_p_cells);
  p_grid_size.resize(num_p_cells);
  particles.resize(num_particles);
  particle_vertices.resize(num_particles * 4);
  particle_VA.create(num_particles * 4);
  particle_VA.setPrimitiveType(sf::Quads);
  std::fill(p_grid_size.begin(), p_grid_size.end(), 0);

  for (uint32_t j = 0; j < f_height; j++) {
    for (uint32_t i = 0; i < f_width; i++) {

      const uint32_t f_pos = getFPos(i, j);
      const size_t cell_va_idx = static_cast<size_t>(f_pos << 2);
      const uint32_t scale = render_scale * f_cell_size * DIAMETER;
      const float x = static_cast<float>(i * scale);
      const float y = static_cast<float>(j * scale);

      f_cell_vertices[cell_va_idx + 0].position = sf::Vector2f(x, y);
      f_cell_vertices[cell_va_idx + 1].position = sf::Vector2f(x + scale, y);
      f_cell_vertices[cell_va_idx + 2].position =
          sf::Vector2f(x + scale, y + scale);
      f_cell_vertices[cell_va_idx + 3].position = sf::Vector2f(x, y + scale);
      f_cell_vertices[cell_va_idx + 0].color = sf::Color::Black;
      f_cell_vertices[cell_va_idx + 1].color = sf::Color::Black;
      f_cell_vertices[cell_va_idx + 2].color = sf::Color::Black;
      f_cell_vertices[cell_va_idx + 3].color = sf::Color::Black;

      const size_t line_va_idx = static_cast<size_t>(f_pos << 2);

      line_vertices[line_va_idx + 0].color = sf::Color::Red;
      line_vertices[line_va_idx + 1].color = sf::Color::Red;
      line_vertices[line_va_idx + 2].color = sf::Color::Red;
      line_vertices[line_va_idx + 3].color = sf::Color::Red;
    }
  }
  const float test = std::sqrt(DIAMETER * DIAMETER - RADIUS_SQRD);
  for (uint32_t i = 0; i < particles.size(); i++) {
    const size_t idx = static_cast<size_t>(i) << 2;
    const uint32_t x = i % 500;
    const uint32_t y = i / 500;

    particle_vertices[idx + 0].color = sf::Color::White;
    particle_vertices[idx + 1].color = sf::Color::White;
    particle_vertices[idx + 2].color = sf::Color::White;
    particle_vertices[idx + 3].color = sf::Color::White;
    particles[i] = {static_cast<float>(f_cell_size * 2) + x + RADIUS * (y % 2),
                    static_cast<float>(f_cell_size * 2) + (y * test), 0.0f};
  }
  particleVelToCell();
  updateDensity();
  // particles[0] = {static_cast<float>(f_cell_ratio) + RADIUS + 10,
  // static_cast<float>(f_cell_ratio) + RADIUS};
}

inline const uint32_t Grid::getFPos(const uint32_t x, const uint32_t y) const {
  return y * f_width + x;
}

inline const uint32_t Grid::getPPos(const uint32_t x, const uint32_t y) const {
  return y * p_width + x;
}

inline const uint32_t Grid::P2F(const uint32_t x, const uint32_t y) const {
  return y / f_cell_size * f_width + x / f_cell_size;
}

inline const uint32_t Grid::P2F(const uint32_t p_pos) const {
  const uint32_t x = p_pos % p_width;
  const uint32_t y = p_pos / p_width;
  return P2F(x, y);
}

inline void Grid::pBounds(Particle &p) {
  p.x_pos = std::clamp(p.x_pos, DIAMETER * f_cell_size + RADIUS,
                       static_cast<float>(p_width) -
                           (DIAMETER * f_cell_size + RADIUS));
  p.y_pos = std::clamp(p.y_pos, DIAMETER * f_cell_size + RADIUS,
                       static_cast<float>(p_height) -
                           (DIAMETER * f_cell_size + RADIUS));
}

const float Grid::genRndFloat(const float min, const float max) {
  static std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(min, max);
  return dis(gen);
}

void Grid::setCellType(const Cell_Type cell_type, const uint32_t x,
                       const uint32_t y) {
  if (x < f_width && y < f_height)
    f_grid[getFPos(x, y)].setCellType(cell_type);
}

void Grid::setCellField(float Cell::*field, const float value, const uint32_t x,
                        const uint32_t y) {
  if (x < f_width && y < f_height)
    f_grid[getFPos(x, y)].*field = value;
}

void Grid::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  states.transform *= getTransform();

  target.draw(f_cell_VA, states);
  sf::RenderStates particleStates = states;
  particleStates.texture = &circle_img;
  target.draw(particle_VA, particleStates);
  target.draw(line_VA, states);
}

void Grid::update() {
  updateParticles();
  particleVelToCell();
  updateDensity();
  project();
  updateVAs();
  cellVelToParticle();
}

void Grid::updateDensity() {
  for (Cell &c : f_grid)
    c.density = 0.0f;

  for (Particle &p : particles) {
    const cellData cd = getData(p, false, false);

    f_grid[cd.f_pos[0]].density += cd.diff[0];
    f_grid[cd.f_pos[1]].density += cd.diff[1];
    f_grid[cd.f_pos[2]].density += cd.diff[2];
    f_grid[cd.f_pos[3]].density += cd.diff[3];
  }

  if (rest_density == 0.0f) {
    float density_sum = 0.0f;
    uint32_t sum_fluid_cells = 0;

    for (Cell c : f_grid) {
      if (c.getCellType() == Cell_Type::LIQUID) {
        density_sum += c.density;
        sum_fluid_cells++;
      }
    }

    if (sum_fluid_cells > 1)
      rest_density = (density_sum / sum_fluid_cells);
  }
}

void Grid::copyField(float Cell::*from, float Cell::*to) {
  for (Cell &c : f_grid) {
    c.*to = c.*from;
  }
}

void Grid::updateVAs() {

  for (uint32_t j = 0; j < f_height; j++) {
    for (uint32_t i = 0; i < f_width; i++) {

      const uint32_t pos = getFPos(i, j);
      const size_t cell_va_idx = static_cast<size_t>(pos << 2);

      switch (f_grid[pos].getCellType()) {
      case Cell_Type::LIQUID:
        f_cell_vertices[cell_va_idx + 0].color = sf::Color::Blue;
        f_cell_vertices[cell_va_idx + 1].color = sf::Color::Blue;
        f_cell_vertices[cell_va_idx + 2].color = sf::Color::Blue;
        f_cell_vertices[cell_va_idx + 3].color = sf::Color::Blue;
        break;
      case Cell_Type::AIR:
        f_cell_vertices[cell_va_idx + 0].color = sf::Color::Black;
        f_cell_vertices[cell_va_idx + 1].color = sf::Color::Black;
        f_cell_vertices[cell_va_idx + 2].color = sf::Color::Black;
        f_cell_vertices[cell_va_idx + 3].color = sf::Color::Black;
        break;
      case Cell_Type::SOLID:
        f_cell_vertices[cell_va_idx + 0].color = sf::Color::Green;
        f_cell_vertices[cell_va_idx + 1].color = sf::Color::Green;
        f_cell_vertices[cell_va_idx + 2].color = sf::Color::Green;
        f_cell_vertices[cell_va_idx + 3].color = sf::Color::Green;
        break;
      default:
        break;
      }

      const uint32_t scale = render_scale * f_cell_size * DIAMETER;
      const size_t line_va_idx = static_cast<size_t>(pos << 2);
      const float x0 = static_cast<float>(i * scale);
      const float y0 = static_cast<float>(j * scale);
      const float x = x0 + scale / 2.0f;
      const float y = y0 + scale / 2.0f;

      line_vertices[line_va_idx + 0].position = sf::Vector2f(x0, y);
      line_vertices[line_va_idx + 1].position =
          sf::Vector2f(x0 + f_grid[pos].x_vel / 15, y);

      line_vertices[line_va_idx + 2].position = sf::Vector2f(x, y0);
      line_vertices[line_va_idx + 3].position =
          sf::Vector2f(x, y0 + f_grid[pos].y_vel / 15);
    }
  }

  for (uint32_t i = 0; i < particles.size(); i++) {
    const sf::Vector2f pos = {particles[i].x_pos, particles[i].y_pos};
    const size_t idx = static_cast<size_t>(i) << 2;
    const float radius = render_scale * RADIUS;
    particle_vertices[idx + 0].position = pos + sf::Vector2f(-radius, -radius);
    particle_vertices[idx + 1].position = pos + sf::Vector2f(radius, -radius);
    particle_vertices[idx + 2].position = pos + sf::Vector2f(radius, radius);
    particle_vertices[idx + 3].position = pos + sf::Vector2f(-radius, radius);
    particle_vertices[idx + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
    particle_vertices[idx + 1].texCoords = sf::Vector2f(IMG_SIZE, 0.0f);
    particle_vertices[idx + 2].texCoords = sf::Vector2f(IMG_SIZE, IMG_SIZE);
    particle_vertices[idx + 3].texCoords = sf::Vector2f(0.0f, IMG_SIZE);
  }

  f_cell_VA.update(f_cell_vertices.data());
  particle_VA.update(particle_vertices.data());
  line_VA.update(line_vertices.data());
}

void Grid::project() {
  copyField(&Cell::x_vel, &Cell::p_x_vel);
  copyField(&Cell::y_vel, &Cell::p_y_vel);
  for (uint32_t k = 0; k < NUM_F_SUB_STEP; k++) {
    for (uint32_t f_pos = 0; f_pos < num_f_cells; f_pos++) {
      if (f_grid[f_pos].getCellType() != Cell_Type::LIQUID)
        continue;

      const uint8_t s = f_grid[f_pos + 1].getS() + f_grid[f_pos - 1].getS() +
                        f_grid[f_pos + f_width].getS() +
                        f_grid[f_pos - f_width].getS();

      if (s == 0)
        continue;

      float d = f_grid[f_pos + 1].x_vel - f_grid[f_pos].x_vel +
                f_grid[f_pos + f_width].y_vel - f_grid[f_pos].y_vel;

      if (rest_density > 0.0f) {
        const float compression = f_grid[f_pos].density - rest_density;
        if (compression > 0.0f)
          d -= K * compression;
      }

      const float p = -d / s * OVER_RELAXATION;
      f_grid[f_pos].x_vel -= f_grid[f_pos - 1].getS() * p;
      f_grid[f_pos + 1].x_vel += f_grid[f_pos + 1].getS() * p;
      f_grid[f_pos].y_vel -= f_grid[f_pos - f_width].getS() * p;
      f_grid[f_pos + f_width].y_vel += f_grid[f_pos + f_width].getS() * p;
    }
  }
}

void Grid::addParticlesToMap() {
  std::fill(p_grid_size.begin(), p_grid_size.end(), 0);
  for (Particle &p : particles) {
    pBounds(p);
    const uint32_t x = static_cast<uint32_t>(p.x_pos);
    const uint32_t y = static_cast<uint32_t>(p.y_pos);
    const uint32_t p_pos = getPPos(x, y);
    uint8_t &size = p_grid_size[p_pos];
    if (size <= P_CELL_ARR_SIZE - 1) {
      p_grid[p_pos][size] = &p;
      size++;
    }
  }
}

void Grid::updateParticles() {
  for (Particle &p : particles) {
    p.y_vel += GRAVITY * DT;
    p.x_pos += p.x_vel * DT;
    p.y_pos += p.y_vel * DT;
  }
  addParticlesToMap();
  for (uint i = 0; i < NUM_P_SUB_STEP; i++) {
    for (uint32_t p_pos = 0; p_pos < num_p_cells; p_pos++) {
      const uint8_t v1l = p_grid_size[p_pos];
      if (v1l != 0) {
        collide(p_pos, p_pos - (p_width + 1));
        collide(p_pos, p_pos - (p_width));
        collide(p_pos, p_pos - (p_width - 1));
        collide(p_pos, p_pos - 1);
        collide(p_pos, p_pos);
        collide(p_pos, p_pos + 1);
        collide(p_pos, p_pos + (p_width - 1));
        collide(p_pos, p_pos + (p_width));
        collide(p_pos, p_pos + (p_width + 1));
      }
    }
  }
  for (uint32_t p_pos = 0; p_pos < num_p_cells; p_pos++) {
    const uint8_t v1l = p_grid_size[p_pos];
    if (v1l != 0) {
      collideSolid(p_pos, p_pos - (p_width + 1));
      collideSolid(p_pos, p_pos - (p_width));
      collideSolid(p_pos, p_pos - (p_width - 1));
      collideSolid(p_pos, p_pos - 1);
      collideSolid(p_pos, p_pos);
      collideSolid(p_pos, p_pos + 1);
      collideSolid(p_pos, p_pos + (p_width - 1));
      collideSolid(p_pos, p_pos + (p_width));
      collideSolid(p_pos, p_pos + (p_width + 1));
    }
  }
  for (Particle &p : particles) {
    pBounds(p);
  }
}

inline const std::array<bool, 4>
Grid::getValidArr(const std::array<uint32_t, 4> f_pos, const uint32_t offset) {
  return std::array<bool, 4>{
      f_grid[f_pos[0]].getCellType() != Cell_Type::AIR ||
          f_grid[f_pos[0] - offset].getCellType() != Cell_Type::AIR,
      f_grid[f_pos[1]].getCellType() != Cell_Type::AIR ||
          f_grid[f_pos[1] - offset].getCellType() != Cell_Type::AIR,
      f_grid[f_pos[2]].getCellType() != Cell_Type::AIR ||
          f_grid[f_pos[2] - offset].getCellType() != Cell_Type::AIR,
      f_grid[f_pos[3]].getCellType() != Cell_Type::AIR ||
          f_grid[f_pos[3] - offset].getCellType() != Cell_Type::AIR,
  };
}

inline const float Grid::getPic(float Cell::*feild,
                                const std::array<uint32_t, 4> f_pos,
                                const std::array<float, 4> vd,
                                const float vd_s) {
  return (vd[0] * f_grid[f_pos[0]].*feild + vd[1] * f_grid[f_pos[1]].*feild +
          vd[2] * f_grid[f_pos[2]].*feild + vd[3] * f_grid[f_pos[3]].*feild) /
         vd_s;
}

inline const float Grid::getFlip(float Cell::*feild, float Cell::*pre_feild,
                                 const std::array<uint32_t, 4> f_pos,
                                 const std::array<float, 4> vd,
                                 const float vd_s) {
  return (vd[0] * (f_grid[f_pos[0]].*feild - f_grid[f_pos[0]].*pre_feild) +
          vd[1] * (f_grid[f_pos[1]].*feild - f_grid[f_pos[1]].*pre_feild) +
          vd[2] * (f_grid[f_pos[2]].*feild - f_grid[f_pos[2]].*pre_feild) +
          vd[3] * (f_grid[f_pos[3]].*feild - f_grid[f_pos[3]].*pre_feild)) /
         vd_s;
}

inline const Grid::cellData Grid::getData(Particle &p, const bool mx,
                                          const bool my) {
  const float dx = mx ? 0.0f : RADIUS * f_cell_size;
  const float dy = my ? 0.0f : RADIUS * f_cell_size;

  const float x = std::clamp(p.x_pos, static_cast<float>(f_cell_size),
                             static_cast<float>((f_width - 1) * f_cell_size));
  const float y = std::clamp(p.y_pos, static_cast<float>(f_cell_size),
                             static_cast<float>((f_height - 1) * f_cell_size));
  const uint32_t x0 =
      std::min(static_cast<uint32_t>((x - dx) / f_cell_size), f_width - 2);
  const uint32_t y0 =
      std::min(static_cast<uint32_t>((y - dy) / f_cell_size), f_height - 2);
  const uint32_t x1 = std::min(x0 + 1, f_width - 2);
  const uint32_t y1 = std::min(y0 + 1, f_height - 2);

  const float dx_0 = ((p.x_pos - dx) - (x0 * f_cell_size)) / f_cell_size;
  const float dy_0 = ((p.y_pos - dy) - (y0 * f_cell_size)) / f_cell_size;
  const float dx_1 = 1.0f - dx_0;
  const float dy_1 = 1.0f - dy_0;

  assert(dx_0 <= 1.0f);
  assert(dy_0 <= 1.0f);
  assert(dx_1 <= 1.0f);
  assert(dy_1 <= 1.0f);

  return cellData{
      std::array<uint32_t, 4>{getFPos(x0, y0), getFPos(x1, y0), getFPos(x1, y1),
                              getFPos(x0, y1)},
      std::array<float, 4>{dx_1 * dy_1, dx_0 * dy_1, dx_0 * dy_0, dx_1 * dy_0}};
}

void Grid::cellVelToParticle() {
  for (Particle &p : particles) {
    const cellData cdx = getData(p, true, false);
    const cellData cdy = getData(p, false, true);

    const uint32_t offset_x = f_width;
    const uint32_t offset_y = 1;

    const std::array<bool, 4> vx = getValidArr(cdx.f_pos, offset_x);
    const std::array<bool, 4> vy = getValidArr(cdy.f_pos, offset_y);

    const std::array<float, 4> vdx = {vx[0] * cdx.diff[0], vx[1] * cdx.diff[1],
                                      vx[2] * cdx.diff[2], vx[3] * cdx.diff[3]};
    const float vdx_s = vdx[0] + vdx[1] + vdx[2] + vdx[3];

    const std::array<float, 4> vdy = {
        vy[0] * cdy.diff[0],
        vy[1] * cdy.diff[1],
        vy[2] * cdy.diff[2],
        vy[3] * cdy.diff[3],
    };
    const float vdy_s = vdy[0] + vdy[1] + vdy[2] + vdy[3];

    if (vdx_s != 0.0f) {
      const float pic_x_vel = getPic(&Cell::x_vel, cdx.f_pos, vdx, vdx_s);
      const float flip_x_vel =
          getFlip(&Cell::x_vel, &Cell::p_x_vel, cdx.f_pos, vdx, vdx_s);
      p.x_vel = pic_x_vel * (1.0f - FLIP) + (flip_x_vel + p.x_vel) * FLIP;
    } else {
      p.x_vel = 0.0f;
    }

    if (vdy_s != 0.0f) {
      const float pic_y_vel = getPic(&Cell::y_vel, cdy.f_pos, vdy, vdy_s);
      const float flip_y_vel =
          getFlip(&Cell::y_vel, &Cell::p_y_vel, cdy.f_pos, vdy, vdy_s);
      p.y_vel = pic_y_vel * (1.0f - FLIP) + (flip_y_vel + p.y_vel) * FLIP;
    } else {
      p.y_vel = 0.0f;
    }
  }
}

void Grid::particleVelToCell() {
  std::fill(r_x.begin(), r_x.end(), 0.0f);
  std::fill(r_y.begin(), r_y.end(), 0.0f);

  for (Cell &c : f_grid) {
    c.p_x_vel = c.x_vel;
    c.p_y_vel = c.y_vel;
    c.x_vel = 0.0f;
    c.y_vel = 0.0f;

    if (c.getCellType() != Cell_Type::SOLID)
      c.setCellType(Cell_Type::AIR);
  }

  for (Particle &p : particles) {
    const uint32_t x = static_cast<uint32_t>(p.x_pos);
    const uint32_t y = static_cast<uint32_t>(p.y_pos);
    const uint32_t f_pos = P2F(x, y);
    if (f_grid[f_pos].getCellType() == Cell_Type::AIR)
      f_grid[f_pos].setCellType(Cell_Type::LIQUID);
  }

  for (Particle &p : particles) {
    const cellData cdx = getData(p, true, false);
    const cellData cdy = getData(p, false, true);

    f_grid[cdx.f_pos[0]].x_vel += cdx.diff[0] * p.x_vel;
    f_grid[cdx.f_pos[1]].x_vel += cdx.diff[1] * p.x_vel;
    f_grid[cdx.f_pos[2]].x_vel += cdx.diff[2] * p.x_vel;
    f_grid[cdx.f_pos[3]].x_vel += cdx.diff[3] * p.x_vel;

    f_grid[cdy.f_pos[0]].y_vel += cdy.diff[0] * p.y_vel;
    f_grid[cdy.f_pos[1]].y_vel += cdy.diff[1] * p.y_vel;
    f_grid[cdy.f_pos[2]].y_vel += cdy.diff[2] * p.y_vel;
    f_grid[cdy.f_pos[3]].y_vel += cdy.diff[3] * p.y_vel;

    r_x[cdx.f_pos[0]] += cdx.diff[0];
    r_x[cdx.f_pos[1]] += cdx.diff[1];
    r_x[cdx.f_pos[2]] += cdx.diff[2];
    r_x[cdx.f_pos[3]] += cdx.diff[3];

    r_y[cdy.f_pos[0]] += cdy.diff[0];
    r_y[cdy.f_pos[1]] += cdy.diff[1];
    r_y[cdy.f_pos[2]] += cdy.diff[2];
    r_y[cdy.f_pos[3]] += cdy.diff[3];
  }
  for (uint32_t j = 0; j < f_height; j++) {
    for (uint32_t i = 0; i < f_width; i++) {
      const uint32_t f_pos = getFPos(i, j);
      f_grid[f_pos].x_vel =
          r_x[f_pos] == 0.0f ? 0.0f : f_grid[f_pos].x_vel / r_x[f_pos];
      f_grid[f_pos].y_vel =
          r_y[f_pos] == 0.0f ? 0.0f : f_grid[f_pos].y_vel / r_y[f_pos];

      if (f_grid[f_pos].getCellType() == Cell_Type::SOLID ||
          (i > 0 && f_grid[f_pos - 1].getCellType() == Cell_Type::SOLID))
        f_grid[f_pos].x_vel = f_grid[f_pos].p_x_vel;
      if (f_grid[f_pos].getCellType() == Cell_Type::SOLID ||
          (j > 0 && f_grid[f_pos - f_width].getCellType() == Cell_Type::SOLID))
        f_grid[f_pos].y_vel = f_grid[f_pos].p_y_vel;
    }
  }
  for (uint32_t f_pos = 0; f_pos < num_f_cells; f_pos++) {
  }
}

void inline Grid::collideSolid(const uint32_t pos1, const uint32_t pos2) {
  if (pos2 < num_p_cells) {
    const uint8_t v1l = p_grid_size[pos1];
    for (uint32_t i = 0; i < v1l; i++) {
      Particle &p = *p_grid[pos1][i];
      const uint32_t x = (pos2 % p_width) / f_cell_size;
      const uint32_t y = (pos2 / p_width) / f_cell_size;
      const uint32_t f_pos2 = getFPos(x, y);
      
      if (f_grid[f_pos2].getCellType() == Cell_Type::SOLID) {
        const uint32_t x0 = x * f_cell_size;
        const uint32_t y0 = y * f_cell_size;
        const uint32_t x1 = (x + 1) * f_cell_size;
        const uint32_t y1 = (y + 1) * f_cell_size;
        const float closest_x =
            std::clamp(p.x_pos, static_cast<float>(x0), static_cast<float>(x1));
        const float closest_y =
            std::clamp(p.y_pos, static_cast<float>(y0), static_cast<float>(y1));
        const sf::Vector2f d3 = {p.x_pos - closest_x, p.y_pos - closest_y};
        const float d2 = (d3.x * d3.x) + (d3.y * d3.y);
        if (d2 <= (RADIUS_SQRD - PADDING)) {
          if (d2 < PADDING) {
            const float overlap_left = p.x_pos - x0;
            const float overlap_right = x1 - p.x_pos;
            const float overlap_top = p.y_pos - y0;
            const float overlap_bottom = y1 - p.y_pos;
            const float min_overlap = std::min(
                {overlap_left, overlap_right, overlap_top, overlap_bottom});

            if (min_overlap == overlap_left)
              p.x_pos = x0 - RADIUS;
            else if (min_overlap == overlap_right)
              p.x_pos = x1 + RADIUS;
            else if (min_overlap == overlap_top)
              p.y_pos = y0 - RADIUS;
            else if (min_overlap == overlap_bottom)
              p.y_pos = y1 + RADIUS;
          } else {
            const float d = std::sqrt(d2);
            const sf::Vector2f dir = d3 / d;
            const float delta = RADIUS - d;
            p.x_pos += delta * dir.x;
            p.y_pos += delta * dir.y;
          }
          if (std::abs(d3.x) > PADDING)
            p.x_vel = 0.0f;
          if (std::abs(d3.y) > PADDING)
            p.y_vel = 0.0f;
        }
      }
    }
  }
}

void inline Grid::collide(const uint32_t pos1, const uint32_t pos2) {
  if (pos2 < num_p_cells) {
    const uint8_t v1l = p_grid_size[pos1];
    const uint8_t v2l = p_grid_size[pos2];
    if (v2l > 0) {
      for (uint32_t i = 0; i < v1l; i++) {
        Particle &p1 = *p_grid[pos1][i];

        for (uint32_t j = 0; j < v2l; j++) {
          Particle &p2 = *p_grid[pos2][j];
          if (p_grid[pos1][i] == p_grid[pos2][j])
            continue;

          const sf::Vector2f d3 = {p2.x_pos - p1.x_pos, p2.y_pos - p1.y_pos};
          const float d2 = d3.x * d3.x + d3.y * d3.y;
          if (d2 < (DIAMETER_SQRD - PADDING) && d2 > PADDING) {
            const float d = std::sqrt(d2);
            const sf::Vector2f dir = d3 / d;
            const float delta = 0.5f * (d - DIAMETER);
            p1.x_pos += delta * dir.x;
            p1.y_pos += delta * dir.y;
            p2.x_pos += -delta * dir.x;
            p2.y_pos += -delta * dir.y;
          }
        }
      }
    }
  }
}