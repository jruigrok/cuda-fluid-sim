#include "Grid.hpp"

Grid::Grid(uint32_t width_, uint32_t height_, uint32_t cellSize_):
    width(width_),
    height(height_),
    cellSize(cellSize_)
{
    grid.resize(width * height);
    cellVertices.resize(width * height * 4);
    lineVertices.resize(width * height * 2);
    cellVA.create(width * height * 4);
    lineVA.create(width * height * 2);
    cellVA.setPrimitiveType(sf::Quads);
    lineVA.setPrimitiveType(sf::Lines);

    for (uint32_t j = 0; j < height; j++) {
        for (uint32_t i = 0; i < width; i++) {
            const uint32_t pos = getPos(i, j);
            if (i == 0 || j == 0 || j == height - 1) {
                grid[pos].cell_type = Cell_Type::SOLID;
            } else {
                grid[pos].cell_type = Cell_Type::LIQUID;
                //grid[pos].density_0 = static_cast<float>(std::rand()) / RAND_MAX;
            }
        }
    }

    for (uint64_t i = 0; i < 16; i++) {
        for (uint64_t j = 0; j < 16; j++) {
            grid[getPos(75 + i, 17 + j)].cell_type = Cell_Type::SOLID;
        }
    }

    initializeVAs();
}

inline uint32_t Grid::getPos(const uint32_t i, const uint32_t j) const {
    return j * width + i;
}

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    target.draw(cellVA, states);
    //target.draw(lineVA, states);
}

void Grid::update() {
    updateVAs(x_vel_buf0, y_vel_buf0, density_buf0);
    addSource(x_vel_buf0, y_vel_buf0, density_buf0);
    project(x_vel_buf0, y_vel_buf0);
    solveAdvection(x_vel_buf1, y_vel_buf1, density_buf1, x_vel_buf0, y_vel_buf0, density_buf0);
    std::swap(x_vel_buf1, x_vel_buf0);
    std::swap(y_vel_buf1, y_vel_buf0);
    std::swap(density_buf1, density_buf0);
    //std::cout << std::to_string(totalFluid()) << std::endl;
}

void Grid::addSource(float Cell::* x_vel, float Cell::* y_vel, float Cell::* density) {
    for (uint32_t i = 0; i < 48; i++) {
        grid[getPos(1, 1 + i)].*x_vel = 10.0;
    }
    for (uint32_t i = 0; i < 10; i++) {
        grid[getPos(2, 21 + i)].*density_buf0 = 10.0;
    }
}

void Grid::initializeVAs() {
    for (uint32_t j = 0; j < height; j++) {
        for (uint32_t i = 0; i < width; i++) {
            const uint32_t pos = getPos(i, j);
            const size_t cell_va_idx = static_cast<size_t>(pos << 2);
            const float x = static_cast<float>(i * cellSize);
            const float y = static_cast<float>(j * cellSize);
            cellVertices[cell_va_idx + 0].position = sf::Vector2f(x, y);
            cellVertices[cell_va_idx + 1].position = sf::Vector2f(x + cellSize, y);
            cellVertices[cell_va_idx + 2].position = sf::Vector2f(x + cellSize, y + cellSize);
            cellVertices[cell_va_idx + 3].position = sf::Vector2f(x, y + cellSize);
            const size_t line_va_idx = static_cast<size_t>(pos << 1);
            lineVertices[line_va_idx + 0].color = sf::Color::Red;
            lineVertices[line_va_idx + 1].color = sf::Color::Red;
        }
    }    
}

void Grid::updateVAs(float Cell::* x_vel, float Cell::* y_vel, float Cell::* density) {
    for (uint32_t j = 0; j < height; j++) {
        for (uint32_t i = 0; i < width; i++) {
            const uint32_t pos = getPos(i, j);
            const size_t cell_va_idx = static_cast<size_t>(pos << 2);
            if (grid[pos].cell_type == Cell_Type::LIQUID) {
                const uint8_t val = std::min(static_cast<uint32_t>(std::round(std::sqrt(grid[pos].*y_vel * grid[pos].*y_vel + grid[pos].*x_vel * grid[pos].*x_vel) * 20)), static_cast<uint32_t>(255));
                //const uint8_t val = 255;
                const sf::Color liquidColor = {val,val,val,std::min(static_cast<sf::Uint8>(std::round(grid[pos].*density * 255)), static_cast<sf::Uint8>(255))};
                cellVertices[cell_va_idx + 0].color = liquidColor;
                cellVertices[cell_va_idx + 1].color = liquidColor;
                cellVertices[cell_va_idx + 2].color = liquidColor;
                cellVertices[cell_va_idx + 3].color = liquidColor;  
            } else {
                const sf::Color solidColor = sf::Color::Green;
                cellVertices[cell_va_idx + 0].color = solidColor;
                cellVertices[cell_va_idx + 1].color = solidColor;
                cellVertices[cell_va_idx + 2].color = solidColor;
                cellVertices[cell_va_idx + 3].color = solidColor;
            }
            const size_t line_va_idx = static_cast<size_t>(pos << 1);
            const float x = static_cast<float>(i * cellSize + (cellSize >> 1));
            const float y = static_cast<float>(j * cellSize + (cellSize >> 1));
            lineVertices[line_va_idx + 0].position = sf::Vector2f(x, y);
            lineVertices[line_va_idx + 1].position = sf::Vector2f(x + grid[pos].*x_vel, y + grid[pos].*y_vel);
        }
    }
    cellVA.update(cellVertices.data());
    lineVA.update(lineVertices.data());
}

// void Grid::solveDiffusion(float Cell::* field, float Cell::* pastfield) {
//     float a = dt * diff * width * height;

//     for (uint32_t k = 0; k < 20; k++) {
//         for (uint32_t j = 1; j < height - 1; j++) {
//             for (uint32_t i = 1; i < width - 1; i++) {
//                 uint32_t pos = getPos(i, j);
//                 grid[pos].*field = (grid[pos].*pastfield + a *
//                                     (grid[pos - 1].*field +
//                                     grid[pos + 1].*field +
//                                     grid[pos - width].*field +
//                                     grid[pos + width].*field)) / (1 + 4 * a);
//             }
//         }
//     }
// }

const float Grid::sample(const float x, const float y, float Cell::* field, const float dx, const float dy) const {
    const uint32_t x0 = std::clamp(static_cast<uint32_t>(x - dx), 1u, width - 2);
    const uint32_t y0 = std::clamp(static_cast<uint32_t>(y - dy), 1u, height - 2);
    const uint32_t x1 = std::min(static_cast<uint32_t>(x0 + 1), width - 2);
    const uint32_t y1 = std::min(static_cast<uint32_t>(y0 + 1), height - 2);
    const float tx = (x - dx) - x0;
    const float ty = (y - dy) - y0;
    const float sx = 1 - tx;
    const float sy = 1 - ty;

    return  sx * sy * grid[getPos(x0, y0)].*field + 
            tx * sy * grid[getPos(x1, y0)].*field + 
            tx * ty * grid[getPos(x1, y1)].*field + 
            sx * ty * grid[getPos(x0, y1)].*field; 
}

void Grid::solveAdvection(float Cell::* x_vel_new, float Cell::* y_vel_new, float Cell::* density_new, float Cell::* x_vel, float Cell::* y_vel, float Cell::* density) {
    for (uint32_t j = 1; j < height; j++) {
        for (uint32_t i = 1; i < width; i++) {
            const uint32_t pos = getPos(i, j);
            if (grid[pos].cell_type == Cell_Type::SOLID)
                continue;
            
            if (grid[pos - 1].cell_type == Cell_Type::LIQUID && j < height - 1) {
                const float x = i - dt * grid[pos].*x_vel;

                const float y = (j + 0.5f) - (dt * (grid[pos].*y_vel + 
                                grid[pos - 1].*y_vel + 
                                grid[pos + width - 1].*y_vel + 
                                grid[pos + width].*y_vel) / 4.0f);

                grid[pos].*x_vel_new = sample(x, y, x_vel, 0.0f, 0.5f);
            }

            if (grid[pos - width].cell_type == Cell_Type::LIQUID && i < width - 1) {
                const float x = (i + 0.5f) - (dt * (grid[pos].*x_vel + 
                                grid[pos + 1].*x_vel + 
                                grid[pos - width + 1].*x_vel + 
                                grid[pos - width].*x_vel) / 4.0f);

                const float y = j - dt * grid[pos].*y_vel;

                grid[pos].*y_vel_new = sample(x, y, y_vel, 0.5f, 0.0f);
            }
            
            if (i < width - 1 && j < height - 1) {
                const float x = (i + 0.5f) - (dt * (grid[pos].*x_vel + grid[pos + 1].*x_vel) / 2.0f);
                const float y = (j + 0.5f) - (dt * (grid[pos].*y_vel + grid[pos + width].*y_vel) / 2.0f);
                
                grid[pos].*density_new = std::clamp(sample(x, y, density, 0.5f, 0.5f), 0.0f, 1.0f);
            }

        }
    }
}

void Grid::project(float Cell::* x_vel, float Cell::* y_vel) {
    for (uint32_t k = 0; k < num_iter; k++) {
        for (uint32_t j = 1; j < height - 1; j++) {
            for (uint32_t i = 1; i < width - 1; i++) {
                const uint32_t pos = getPos(i, j);
                if (grid[pos].cell_type == Cell_Type::SOLID)
                    continue;

                const float d = over_relaxation * (grid[pos + 1].*x_vel - 
                                grid[pos].*x_vel + 
                                grid[pos + width].*y_vel - 
                                grid[pos].*y_vel);

                const uint8_t s =   grid[pos + 1].cell_type + 
                                    grid[pos - 1].cell_type + 
                                    grid[pos + width].cell_type + 
                                    grid[pos - width].cell_type;
                
                if (s == 0)
                    continue;
                                
                const float p = -d / s;
                grid[pos].*x_vel -= grid[pos - 1].cell_type * p; 
                grid[pos + 1].*x_vel += grid[pos + 1].cell_type * p; 
                grid[pos].*y_vel -= grid[pos - width].cell_type * p;
                grid[pos + width].*y_vel += grid[pos + width].cell_type * p;
            }
        }
    }
}

double Grid::totalFluid() const {
    double fluid = 0;
    for (uint64_t i = 0; i < width * height; i++) {
        fluid += grid[i].density_0;
    }
    return fluid;
}