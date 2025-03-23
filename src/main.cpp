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
    static constexpr uint32_t width = 300;
    static constexpr uint32_t height = 50;
    static constexpr uint32_t cellSize = 10;
    static constexpr uint32_t frameLimit = 60;

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

        window.clear();
        window.draw(grid, renderState);
        grid.update();
        window.display();
    }
    return 0;
}