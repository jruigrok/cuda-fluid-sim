#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <functional>
#include <chrono>
#include <ViewPort.hpp>
#include <Grid.hpp>

int main(int argc, char* argv[]) {
    static constexpr uint32_t SCREEN_WIDTH = 1980 / 2;
    static constexpr uint32_t SCREEEN_HEIGHT = 1080 / 2;
    static constexpr uint32_t WIDTH = 600;
    static constexpr uint32_t HEIGHT = 100;
    static constexpr uint32_t CELL_SIZE = 10;
    static constexpr uint32_t FRAME_LIMIT = 165;

    sf::Clock clock;

    int frames = 0;
    float elapsed_time = 0.0f;

    sf::Vector2i mouse;
    sf::Texture circle_img;
    sf::RenderStates render_state;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEEN_HEIGHT), "Fluid Sim");
    std::vector<sf::RenderStates*> states_vector = { &render_state };
    ViewPort viewPort(states_vector, { 0,0 }, 0.5);

    Grid grid(WIDTH, HEIGHT, CELL_SIZE, 50000);

    window.setFramerateLimit(FRAME_LIMIT);

    // Init grid

    for (uint32_t j = 0; j < HEIGHT; j++) {
        for (uint32_t i = 0; i < WIDTH; i++) {
            if (i == 0 || j == 0 || j == HEIGHT - 1 || i == WIDTH - 1) {
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
        elapsed_time += deltaTime.asSeconds();
        frames++;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            } else if (event.type == sf::Event::Resized) {
                sf::View view(sf::FloatRect(0, 0, event.size.width, event.size.height));
                window.setView(view);
            }
            viewPort.handleEvent(event);
        }

        if (elapsed_time >= 1.0f) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            elapsed_time = 0.0f;
        }

        window.clear();
        window.draw(grid, render_state);
        grid.update();
        window.display();
    }
    return 0;
}