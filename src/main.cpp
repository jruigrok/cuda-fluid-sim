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
    static constexpr uint32_t WIDTH = 360;
    static constexpr uint32_t HEIGHT = 150;
    static constexpr uint32_t F_CELL_RATIO = 3;
    static constexpr uint32_t NUM_PARTICLES = 100000;
    static constexpr uint32_t RENDER_SCALE = 1;
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

    Grid grid(WIDTH, HEIGHT, F_CELL_RATIO, NUM_PARTICLES, RENDER_SCALE);

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

        if (viewPort.getMouseDown()) {
            
            const sf::Vector2f mouse_pos = viewPort.getRelativeMousePos();
            std::cout << mouse_pos.x << std::endl;
            const uint32_t x = static_cast<uint32_t>(mouse_pos.x / F_CELL_RATIO);
            const uint32_t y = static_cast<uint32_t>(mouse_pos.y / F_CELL_RATIO);
            grid.setCellType(Cell_Type::SOLID, static_cast<uint32_t>(mouse_pos.x / F_CELL_RATIO), static_cast<uint32_t>(mouse_pos.y / F_CELL_RATIO));
            grid.setCellField(&Cell::x_vel, 0, x, y);
            grid.setCellField(&Cell::y_vel, 0, x, y);
            grid.setCellField(&Cell::p_x_vel, 0, x, y);
            grid.setCellField(&Cell::p_y_vel, 0, x, y);
            grid.setCellField(&Cell::density, 0, x, y);
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