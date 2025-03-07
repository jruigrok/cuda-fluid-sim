#include "Grid.hpp"

Grid::Grid(uint32_t width_, uint32_t height_, uint32_t cellSize_):
    width(width_),
    height(height_),
    cellSize(cellSize_)
{
    grid = new Cell[width * height];
    cellVertices = new sf::Vertex[width * height * 4];
    lineVertices = new sf::Vertex[width * height * 2];
    cellVA.create(width * height * 4);
    lineVA.create(width * height * 2);
    cellVA.setPrimitiveType(sf::Quads);
    lineVA.setPrimitiveType(sf::Lines);
    
    initializeVAs();
}

Grid::~Grid() {
    delete[] grid;
    delete[] cellVertices;
    delete[] lineVertices;
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
    swap(x_vel_buf0, x_vel_buf1);
    swap(y_vel_buf0, y_vel_buf1);
    swap(density_buf0, density_buf1);
    addSource(x_vel_buf0, y_vel_buf0);
    solveDiffusion(x_vel_buf1, x_vel_buf0, 1);
    solveDiffusion(y_vel_buf1, y_vel_buf0, 2);
    project();
    solveAdvection(x_vel_buf1, x_vel_buf1,1);
    solveAdvection(y_vel_buf1, y_vel_buf1,2);
    project();
    solveDiffusion(density_buf0, density_buf1, 0);
    solveAdvection(density_buf0, density_buf0, 0);
    updateVAs();
    //std::cout << std::to_string(totalFluid()) << std::endl;
}

void Grid::addSource(float Cell::* x_vel, float Cell::* y_vel) {
    for (uint64_t i = 0; i < 10; i++) {
        //grid[getPos(50, 50 + i)].*x_vel = 40.0;
        //grid[getPos(150, 150 - i)].*x_vel = -40.0;
        //grid[getPos(100 - i, 100)].*y_vel = -40.0;
        //grid[getPos(50, 150 - i)].*x_vel = 40.0;
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
    
    updateVAs();
    
}

inline void Grid::swap(float Cell::*& field, float Cell::*& pastfield) {
    float Cell::* temp = field;
    field = pastfield;
    pastfield = temp;

    // for (uint32_t j = 1; j < height - 1; j++) {
    //     for (uint32_t i = 1; i < width - 1; i++) {
    //         const uint32_t pos = getPos(i, j);
    //         grid[pos].*pastfield = grid[pos].*field;
    //     }
    // }
}

void Grid::updateVAs() {
    for (uint32_t j = 0; j < height; j++) {
        for (uint32_t i = 0; i < width; i++) {
            const uint32_t pos = getPos(i, j);
            const size_t cell_va_idx = static_cast<size_t>(pos << 2);
            //const uint8_t val = std::min(static_cast<uint32_t>(std::round(std::sqrt(grid[pos].y_vel_0 * grid[pos].y_vel_0 + grid[pos].x_vel_0 * grid[pos].x_vel_0) * 20)), static_cast<uint32_t>(255));
            const uint8_t val = 255;
            const sf::Color cellColor = {val,val,val,std::min(static_cast<sf::Uint8>(std::round(grid[pos].density_0 * 255)), static_cast<sf::Uint8>(255))};
            cellVertices[cell_va_idx + 0].color = cellColor;
            cellVertices[cell_va_idx + 1].color = cellColor;
            cellVertices[cell_va_idx + 2].color = cellColor;
            cellVertices[cell_va_idx + 3].color = cellColor;
            const size_t line_va_idx = static_cast<size_t>(pos << 1);
            const float x = static_cast<float>(i * cellSize + (cellSize >> 1));
            const float y = static_cast<float>(j * cellSize + (cellSize >> 1));
            lineVertices[line_va_idx + 0].position = sf::Vector2f(x, y);
            lineVertices[line_va_idx + 1].position = sf::Vector2f(x + grid[pos].x_vel_0, y + grid[pos].y_vel_0);
        }
    }
    cellVA.update(cellVertices);
    //lineVA.update(lineVertices);
}

void Grid::solveDiffusion(float Cell::* field, float Cell::* pastfield, uint32_t p) {
    float a = dt * diff * width * height;

    for (uint32_t k = 0; k < 20; k++) {
        for (uint32_t j = 1; j < height - 1; j++) {
            for (uint32_t i = 1; i < width - 1; i++) {
                uint32_t pos = getPos(i, j);
                grid[pos].*field = (grid[pos].*pastfield + a *
                                    (grid[pos - 1].*field +
                                    grid[pos + 1].*field +
                                    grid[pos - width].*field +
                                    grid[pos + width].*field)) / (1 + 4 * a);
            }
        }
        setBounds(p, field);
    }
}

void Grid::solveAdvection(float Cell::* field, float Cell::* pastfield, uint32_t p) {
    
    const float dt0 = dt * width;

    for (uint32_t j = 1; j < height - 1; j++) {
        for (uint32_t i = 1; i < width - 1; i++) {
            const uint32_t pos = getPos(i, j);
            const float x = std::min(std::max(i - dt0 * grid[pos].x_vel_0,0.5f), width - 0.5f);
            const float y = std::min(std::max(j - dt0 * grid[pos].y_vel_0,0.5f), height - 0.5f);
            const uint32_t i0 = static_cast<uint32_t>(x);
            const uint32_t j0 = static_cast<uint32_t>(y);
            const uint32_t i1 = i0 + 1;
            const uint32_t j1 = j0 + 1;
            const float s1 = x - i0;
            const float s0 = 1 - s1;
            const float t1 = y - j0;
            const float t0 = 1 - t1;
            grid[pos].*field = s0 * (t0 * grid[getPos(i0, j0)].*pastfield + t1 * grid[getPos(i0, j1)].*pastfield) +
                s1 * (t0 * grid[getPos(i1, j0)].*pastfield + t1 * grid[getPos(i1, j1)].*pastfield);
        }
    }
    setBounds(1, field);
}

void Grid::setBounds(uint32_t b, float Cell::*field) {
    for (uint32_t i = 1; i < width - 1; i++) {
        grid[getPos(i, 0)].*field = (b == 2) ? -1 * grid[getPos(i, 1)].*field : grid[getPos(i, 1)].*field;
        grid[getPos(i, height - 1)].*field = (b == 2) ? -1 * grid[getPos(i, height - 2)].*field : grid[getPos(i, height - 2)].*field;
    }

    for (uint32_t i = 1; i < height - 1; i++) {
        grid[getPos(0, i)].*field = (b == 1) ? -1 * grid[getPos(1, i)].*field : grid[getPos(1, i)].*field;
        grid[getPos(width - 1, i)].*field = (b == 1) ? -1 * grid[getPos(width - 2, i)].*field : grid[getPos(width - 2, i)].*field;
    }

    // Corners
    grid[getPos(0, 0)].*field = 0.5f * (grid[getPos(1, 0)].*field + grid[getPos(0, 1)].*field);
    grid[getPos(0, height-1)].*field = 0.5f * (grid[getPos(1, height - 1)].*field + grid[getPos(0, height - 2)].*field);
    grid[getPos(width-1, 0)].*field = 0.5f * (grid[getPos(width - 2, 0)].*field + grid[getPos(width - 1, 1)].*field);
    grid[getPos(width-1, height-1)].*field = 0.5f * (grid[getPos(width - 2, height - 1)].*field + grid[getPos(width - 1, height - 2)].*field);
}

void Grid::project() {
    const float h = 1.0 / width;
    for (uint32_t j = 1; j < height - 1; j++) {
        for (uint32_t i = 1; i < width - 1; i++) {
            const uint32_t pos = getPos(i, j);
            grid[pos].divergence = -0.5f * h * (grid[pos + 1].x_vel_0 - grid[pos - 1].x_vel_0 + grid[pos + width].y_vel_0 - grid[pos - width].y_vel_0);
            grid[pos].curl = 0;
        }
    }
    setBounds(0, &Cell::curl);
    setBounds(0, &Cell::divergence);


    for (uint32_t k = 0; k < 20; k++) {
        for (uint32_t j = 1; j < height - 1; j++) {
            for (uint32_t i = 1; i < width - 1; i++) {
                const uint32_t pos = getPos(i, j);
                grid[pos].curl = (grid[pos].divergence + grid[pos - 1].curl + grid[pos + 1].curl + grid[pos - width].curl + grid[pos + width].curl)/4.0f;
            }
        }
        setBounds(0, &Cell::curl);
    }

    for (uint32_t j = 1; j < height - 1; j++) {
        for (uint32_t i = 1; i < width - 1; i++) {
            const uint32_t pos = getPos(i, j);
            grid[pos].x_vel_0 -= 0.5f * (grid[pos + 1].curl - grid[pos - 1].curl) / h;
            grid[pos].y_vel_0 -= 0.5f * (grid[pos + width].curl - grid[pos - width].curl) / h;
        }
    }
    setBounds(1, &Cell::x_vel_0);
    setBounds(2, &Cell::y_vel_0);
}

double Grid::totalFluid() const {
    double fluid = 0;
    for (uint64_t i = 0; i < width * height; i++) {
        fluid += grid[i].density_0;
    }
    return fluid;
}