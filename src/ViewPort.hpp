#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <vector>

class ViewPort {
public:
	ViewPort(std::vector<sf::RenderStates*> statesV_, sf::Vector2f pos_, float zoom_);

	void updateStates(sf::Transform& transform);

	void zoom(float factor);

	void move(sf::Vector2f pos_);

	void moveTo(sf::Vector2f pos_);

	sf::Vector2f getTruePos(sf::Vector2f pos_) const;

	void zoomOnPoint(float factor, sf::Vector2f pos_);

	void handleEvent(sf::Event event);

	float getScale() const;

	sf::Vector2f getPos() const;

private:

	std::vector<sf::RenderStates*> statesV;
	sf::Transform transform;
	sf::Vector2f pos;
	sf::Vector2f anchorPos;
	bool mouseDown;
	float scale;
	static constexpr float scrollZoomMag = 0.8;
	sf::Vector2f mousePos;
};
