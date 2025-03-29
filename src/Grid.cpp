#include "Grid.hpp"

const std::string_view Grid::CIRCLE_PNG_FILEPATH = CIRCLE_IMG_PATH;

Grid::Grid(const uint32_t cell_width_, const uint32_t cell_height_, const uint32_t cell_size_, const uint32_t num_particles_):
    cell_width(cell_width_),
    cell_height(cell_height_),
    cell_size(cell_size_),
    num_cells(cell_width * cell_height),
    num_particles(num_particles_),
    width(cell_size * cell_width),
    height(cell_size * cell_height),
    radius(cell_size / 2.0f),
    cell_size_sqrd(cell_size * cell_size)
{
    if (!circle_img.loadFromFile(CIRCLE_PNG_FILEPATH.data())) {
        throw std::runtime_error("Error file not found " + *CIRCLE_PNG_FILEPATH.data());
    }
    circle_img.setSmooth(true);
    circle_img.generateMipmap();

    grid.resize(cell_width * cell_height);
    particle_map.resize(cell_width * cell_height);
    particle_map_size.resize(cell_width * cell_height);
    particles.resize(num_particles);
    cell_vertices.resize(cell_width * cell_height * 4);
    particle_vertices.resize(num_particles * 4);
    line_vertices.resize(cell_width * cell_height * 2);

    cell_VA.create(cell_width * cell_height * 4);
    particle_VA.create(num_particles * 4);
    line_VA.create(cell_width * cell_height * 2);
    cell_VA.setPrimitiveType(sf::Quads);
    particle_VA.setPrimitiveType(sf::Quads);
    line_VA.setPrimitiveType(sf::Lines);

    std::fill(particle_map_size.begin(), particle_map_size.end(), 0);
    for (uint32_t j = 0; j < cell_height; j++) {
        for (uint32_t i = 0; i < cell_width; i++) {
            const uint32_t pos = getPos(i, j);
            const size_t cell_va_idx = static_cast<size_t>(pos << 2);
            const float x = static_cast<float>(i * cell_size);
            const float y = static_cast<float>(j * cell_size);
            cell_vertices[cell_va_idx + 0].position = sf::Vector2f(x, y);
            cell_vertices[cell_va_idx + 1].position = sf::Vector2f(x + cell_size, y);
            cell_vertices[cell_va_idx + 2].position = sf::Vector2f(x + cell_size, y + cell_size);
            cell_vertices[cell_va_idx + 3].position = sf::Vector2f(x, y + cell_size);
            cell_vertices[cell_va_idx + 0].color = sf::Color::Black;
            cell_vertices[cell_va_idx + 1].color = sf::Color::Black;
            cell_vertices[cell_va_idx + 2].color = sf::Color::Black;
            cell_vertices[cell_va_idx + 3].color = sf::Color::Black;
            const size_t line_va_idx = static_cast<size_t>(pos << 1);
            line_vertices[line_va_idx + 0].color = sf::Color::Red;
            line_vertices[line_va_idx + 1].color = sf::Color::Red;
        }
    }
    for (uint32_t i = 0; i < particles.size(); i++) {
        const size_t idx = static_cast<size_t>(i) << 2;
        particle_vertices[idx + 0].color = sf::Color::White;
        particle_vertices[idx + 1].color = sf::Color::White;
        particle_vertices[idx + 2].color = sf::Color::White;
        particle_vertices[idx + 3].color = sf::Color::White;
        particles[i] = {genRndFloat(0.0f, width), genRndFloat(0.0f, height), 0.0f, 0.0f};
    }
    for (int i = 0; i < 5; ++i) {
        float randomValue = genRndFloat(0.0f, width);
        std::cout << "Random value " << i + 1 << ": " << randomValue << std::endl;
    }
    particles[0].x_pos = 166 * 10;
    particles[0].y_pos = 49 * 10;
}

inline const uint32_t Grid::getPos(const uint32_t x, const uint32_t y) const {
    return y * cell_width + x;
}

float Grid::genRndFloat(const float min, const float max) {
    static std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

const Cell& Grid::getCell(const uint32_t x, const uint32_t y) {
    return grid[getPos(x,y)];
}

void Grid::setCellType(const Cell_Type cell_type, const uint32_t x, const uint32_t y) {
    grid[getPos(x,y)].cell_type = cell_type;
}

void Grid::setField(const Field field, const float value, const uint32_t x, const uint32_t y) {
    switch (field)
    {
    case Field::X_VEL:
        grid[getPos(x,y)].*x_vel_buf0 = value;
        break;
    case Field::Y_VEL:
        grid[getPos(x,y)].*y_vel_buf0 = value;
        break;
    default:
        break;
    }
}

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    target.draw(cell_VA, states);
    sf::RenderStates particleStates = states;
    particleStates.texture = &circle_img;
    target.draw(particle_VA, particleStates);
    target.draw(line_VA, states);
}

void Grid::update() {
    addParticlesToMap();
    updateParticles();
    updateVAs(x_vel_buf0, y_vel_buf0);
    project(x_vel_buf0, y_vel_buf0);
    std::swap(x_vel_buf1, x_vel_buf0);
    std::swap(y_vel_buf1, y_vel_buf0);
}

void Grid::updateVAs(float Cell::* x_vel, float Cell::* y_vel) {
    for (uint32_t j = 0; j < cell_height; j++) {
        for (uint32_t i = 0; i < cell_width; i++) {
            const uint32_t pos = getPos(i, j);
            const size_t cell_va_idx = static_cast<size_t>(pos << 2);
            if (grid[pos].cell_type == Cell_Type::LIQUID) {
                const uint8_t val = std::min(static_cast<uint32_t>(std::round(std::sqrt(grid[pos].*y_vel * grid[pos].*y_vel + grid[pos].*x_vel * grid[pos].*x_vel) * 20)), static_cast<uint32_t>(255));
                //const uint8_t val = 255;
                // const sf::Color liquidColor = {val,val,val,std::min(static_cast<sf::Uint8>(std::round(grid[pos].*density * 255)), static_cast<sf::Uint8>(255))};
                // cell_vertices[cell_va_idx + 0].color = liquidColor;
                // cell_vertices[cell_va_idx + 1].color = liquidColor;
                // cell_vertices[cell_va_idx + 2].color = liquidColor;
                // cell_vertices[cell_va_idx + 3].color = liquidColor;  
            } else {
                const sf::Color solidColor = sf::Color::Green;
                cell_vertices[cell_va_idx + 0].color = solidColor;
                cell_vertices[cell_va_idx + 1].color = solidColor;
                cell_vertices[cell_va_idx + 2].color = solidColor;
                cell_vertices[cell_va_idx + 3].color = solidColor;
            }
            const size_t line_va_idx = static_cast<size_t>(pos << 1);
            const float x = static_cast<float>(i * cell_size + (cell_size >> 1));
            const float y = static_cast<float>(j * cell_size + (cell_size >> 1));
            line_vertices[line_va_idx + 0].position = sf::Vector2f(x, y);
            line_vertices[line_va_idx + 1].position = sf::Vector2f(x + grid[pos].*x_vel, y + grid[pos].*y_vel);
        }
    }

    for (uint32_t i = 0; i < particles.size(); i++) {
        const sf::Vector2f pos = {particles[i].x_pos, particles[i].y_pos};
        const size_t idx = static_cast<size_t>(i) << 2;
        particle_vertices[idx + 0].position = pos + sf::Vector2f(-radius, -radius);
        particle_vertices[idx + 1].position = pos + sf::Vector2f(radius, -radius);
        particle_vertices[idx + 2].position = pos + sf::Vector2f(radius, radius);
        particle_vertices[idx + 3].position = pos + sf::Vector2f(-radius, radius);
        particle_vertices[idx + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
        particle_vertices[idx + 1].texCoords = sf::Vector2f(IMG_SIZE, 0.0f);
        particle_vertices[idx + 2].texCoords = sf::Vector2f(IMG_SIZE, IMG_SIZE);
        particle_vertices[idx + 3].texCoords = sf::Vector2f(0.0f, IMG_SIZE);
    }

    cell_VA.update(cell_vertices.data());
    particle_VA.update(particle_vertices.data());
    line_VA.update(line_vertices.data());
}

const float Grid::sample(const float x, const float y, float Cell::* field, const float dx, const float dy) const {
    const uint32_t x0 = std::clamp(static_cast<uint32_t>(x - dx), 1u, cell_width - 2);
    const uint32_t y0 = std::clamp(static_cast<uint32_t>(y - dy), 1u, cell_height - 2);
    const uint32_t x1 = std::min(static_cast<uint32_t>(x0 + 1), cell_width - 2);
    const uint32_t y1 = std::min(static_cast<uint32_t>(y0 + 1), cell_height - 2);
    const float tx = (x - dx) - x0;
    const float ty = (y - dy) - y0;
    const float sx = 1 - tx;
    const float sy = 1 - ty;

    return  sx * sy * grid[getPos(x0, y0)].*field + 
            tx * sy * grid[getPos(x1, y0)].*field + 
            tx * ty * grid[getPos(x1, y1)].*field + 
            sx * ty * grid[getPos(x0, y1)].*field; 
}

// void Grid::solveAdvection(float Cell::* x_vel_new, float Cell::* y_vel_new, float Cell::* density_new, float Cell::* x_vel, float Cell::* y_vel, float Cell::* density) {
//     for (uint32_t i = 1; i < width; i++) {
//         for (uint32_t j = 1; j < height; j++) { 
//             const uint32_t pos = getPos(i, j);
//             if (grid[pos].cell_type == Cell_Type::SOLID)
//                 continue;
            
//             if (grid[pos - 1].cell_type == Cell_Type::LIQUID && j < height - 1) {
//                 const float x = i - DT * grid[pos].*x_vel;

//                 const float y = (j + 0.5f) - (DT * (grid[pos].*y_vel + 
//                                 grid[pos - 1].*y_vel + 
//                                 grid[pos + width - 1].*y_vel + 
//                                 grid[pos + width].*y_vel) / 4.0f);

//                 grid[pos].*x_vel_new = sample(x, y, x_vel, 0.0f, 0.5f);
//             }

//             if (grid[pos - width].cell_type == Cell_Type::LIQUID && i < width - 1) {
//                 const float x = (i + 0.5f) - (DT * (grid[pos].*x_vel + 
//                                 grid[pos + 1].*x_vel + 
//                                 grid[pos - width + 1].*x_vel + 
//                                 grid[pos - width].*x_vel) / 4.0f);

//                 const float y = j - DT * grid[pos].*y_vel;

//                 grid[pos].*y_vel_new = sample(x, y, y_vel, 0.5f, 0.0f);
//             }
            
//             if (i < width - 1 && j < height - 1) {
//                 const float x = (i + 0.5f) - (DT * (grid[pos].*x_vel + grid[pos + 1].*x_vel) / 2.0f);
//                 const float y = (j + 0.5f) - (DT * (grid[pos].*y_vel + grid[pos + width].*y_vel) / 2.0f);
                
//                 grid[pos].*density_new = std::clamp(sample(x, y, density, 0.5f, 0.5f), 0.0f, 1.0f);
//             }

//         }
//     }
// }

void Grid::project(float Cell::* x_vel, float Cell::* y_vel) {
    for (uint32_t k = 0; k < NUM_ITER; k++) {
        for (uint32_t i = 1; i < cell_width - 1; i++) {
            for (uint32_t j = 1; j < cell_height - 1; j++) { 
                const uint32_t pos = getPos(i, j);
                if (grid[pos].cell_type == Cell_Type::SOLID)
                    continue;

                const float d = OVER_RELAXATION * (grid[pos + 1].*x_vel - 
                                grid[pos].*x_vel + 
                                grid[pos + cell_width].*y_vel - 
                                grid[pos].*y_vel);

                const uint8_t s =   grid[pos + 1].cell_type + 
                                    grid[pos - 1].cell_type + 
                                    grid[pos + cell_width].cell_type + 
                                    grid[pos - cell_width].cell_type;
                
                if (s == 0)
                    continue;
                                
                const float p = -d / s;
                grid[pos].*x_vel -= grid[pos - 1].cell_type * p; 
                grid[pos + 1].*x_vel += grid[pos + 1].cell_type * p; 
                grid[pos].*y_vel -= grid[pos - cell_width].cell_type * p;
                grid[pos + cell_width].*y_vel += grid[pos + cell_width].cell_type * p;
            }
        }
    }
}

void Grid::addParticlesToMap() {
    std::fill(particle_map_size.begin(), particle_map_size.end(), 0);
    for (Particle& p : particles) {
        p.x_pos = std::clamp(p.x_pos,radius + PADDING,static_cast<float>(width) - (radius + PADDING));
        p.y_pos = std::clamp(p.y_pos,radius + PADDING,static_cast<float>(height) - (radius + PADDING));
        const uint32_t x = static_cast<uint32_t>(std::floor(p.x_pos / cell_size));
        const uint32_t y = static_cast<uint32_t>(std::floor(p.y_pos / cell_size));
        const uint32_t pos = getPos(x,y);
        uint8_t& size = particle_map_size[pos];
        particle_map[pos][size] = &p;
        size++;
    }
}

void Grid::updateParticles() {
    for (uint32_t j = 0; j < cell_height; j++) {
        for (uint32_t i = 0; i < cell_width; i++) {
            const uint32_t pos = getPos(i,j);
            const uint8_t v1l = particle_map_size[pos];
            if (v1l != 0) {
                collide(pos,pos - (cell_width + 1));
                collide(pos,pos - (cell_width));
                collide(pos,pos - (cell_width - 1));
                collide(pos,pos - 1);
                collide(pos,pos);
                collide(pos,pos + 1);
                collide(pos,pos + (cell_width - 1));
                collide(pos,pos + (cell_width));
                collide(pos,pos + (cell_width + 1));
            }
        }
    }
}

void inline Grid::collide(const uint32_t pos1, const uint32_t pos2) {
    // scuffed
    if (pos2 < num_cells) {
        const uint8_t v1l = particle_map_size[pos1];
        const uint8_t v2l = particle_map_size[pos2];
        if (v2l > 0) {
            for (uint32_t i = 0; i < v1l; i++) {
                Particle& p1 = *particle_map[pos1][i];
                
                for (uint32_t j = 0; j < v2l; j++) {
                    Particle& p2 = *particle_map[pos2][j];
                    const sf::Vector2f d3 = {p2.x_pos - p1.x_pos, p2.y_pos - p1.y_pos};
                    const float d2 = d3.x * d3.x + d3.y * d3.y;
                    if (d2 < (cell_size_sqrd - PADDING) && d2 > PADDING) {
                        const float d = std::sqrt(d2);
                        const sf::Vector2f dir = d3 / d;
                        const float delta = 0.5f * (d - cell_size);
                        p1.x_pos += delta * dir.x;
                        p1.y_pos += delta * dir.y;
                        p2.x_pos += -delta * dir.x;
                        p2.y_pos += -delta * dir.y;
                    }                
                }
            }
        }

        for (uint32_t i = 0; i < v1l; i++) {
            Particle& p1 = *particle_map[pos1][i];

            if (grid[pos2].cell_type == Cell_Type::SOLID) {
                const uint32_t x0 = (pos2 % cell_width) * cell_size;
                const uint32_t y0 = (pos2 / cell_width) * cell_size;
                const uint32_t x1 = x0 + cell_size;
                const uint32_t y1 = y0 + cell_size;
                const float closest_x = std::clamp(p1.x_pos, static_cast<float>(x0), static_cast<float>(x1));
                const float closest_y = std::clamp(p1.y_pos, static_cast<float>(y0), static_cast<float>(y1));
                const sf::Vector2f d3 = {p1.x_pos - closest_x, p1.y_pos - closest_y};
                const float d2 = (d3.x * d3.x) + (d3.y * d3.y);
                if (d2 <= ((radius * radius) - PADDING)) {
                    if (d2 < PADDING) {
                        const float overlap_left = p1.x_pos - x0;
                        const float overlap_right = x1 - p1.x_pos;
                        const float overlap_top = p1.y_pos - y0;
                        const float overlap_bottom = y1 - p1.y_pos;
                        
                        const float min_overlap = std::min({overlap_left, overlap_right, overlap_top, overlap_bottom});

                        if (min_overlap == overlap_left)
                            p1.x_pos = x0 - radius;
                        else if (min_overlap == overlap_right)
                            p1.x_pos = x1 + radius;
                        else if (min_overlap == overlap_top)
                            p1.y_pos = y0 - radius;
                        else if (min_overlap == overlap_bottom)
                            p1.y_pos = y1 + radius;
                    } else {
                        const float d = std::sqrt(d2);
                        const sf::Vector2f dir = d3 / d;
                        const float delta = radius - d;
                        p1.x_pos += delta * dir.x;
                        p1.y_pos += delta * dir.y;
                    }
                }
            }
        }
    } 
}