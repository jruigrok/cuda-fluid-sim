#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <functional>
#include <chrono>
#include <ViewPort.hpp>
#include <Grid.hpp>

int main(int argc, char* argv[]) {
    static constexpr uint32_t screenWidth = 1980 / 2;
    static constexpr uint32_t screenHeight = 1080 / 2;
    static constexpr uint32_t width = 600;
    static constexpr uint32_t height = 100;
    static constexpr uint32_t cellSize = 10;
    static constexpr uint32_t frameLimit = 165;

    sf::Clock clock;

    int frames = 0;
    float elapsedTime = 0.0f;

    sf::Vector2i mouse;
    sf::Texture circleImg;
    sf::RenderStates renderState;
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Fluid Sim");
    std::vector<sf::RenderStates*> statesV = { &renderState };
    ViewPort viewPort(statesV, { 0,0 }, 0.5);

    Grid grid(width, height, cellSize);

    window.setFramerateLimit(frameLimit);

    // Init grid

    for (uint32_t j = 0; j < height; j++) {
        for (uint32_t i = 0; i < width; i++) {
            if (i == 0 || j == 0 || j == height - 1) {
                grid.setCellType(Cell_Type::SOLID, i, j);
            } else {
                grid.setCellType(Cell_Type::LIQUID, i, j);
            }
        }
    }

    for (uint32_t i = 0; i < 32; i++) {
        for (uint32_t j = 0; j < 32; j++) {
            grid.setCellType(Cell_Type::SOLID, 150 + i, 33 + j);
        }
    }

  
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        elapsedTime += deltaTime.asSeconds();
        frames++;
        sf::Event Event;
        while (window.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed)
                window.close();
            else if (Event.type == sf::Event::KeyPressed) {
                if (Event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            } else if (Event.type == sf::Event::Resized) {
                sf::View view(sf::FloatRect(0, 0, Event.size.width, Event.size.height));
                window.setView(view);
            }
            viewPort.handleEvent(Event);
        }

        if (elapsedTime >= 1.0f) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            elapsedTime = 0.0f;
        }

        for (uint32_t i = 1; i < 99; i++) {
            grid.setField(Grid::Field::X_VEL, 10.0f, 1u, i);
        }
        for (uint32_t i = 0; i < 20; i++) {
            grid.setField(Grid::Field::DENSITY, 1.0f, 4u, 39u + i);
        }

        window.clear();
        window.draw(grid, renderState);
        grid.update();
        window.display();
    }
    return 0;
}