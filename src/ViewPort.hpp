#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>

class ViewPort {
public:
  ViewPort(const std::vector<sf::RenderStates *> statesV_,
           const sf::Vector2f pos_, const float zoom_);
  void updateStates(const sf::Transform &transform);
  void zoom(const float factor);
  void move(const sf::Vector2f pos_);
  void moveTo(const sf::Vector2f pos_);
  const sf::Vector2f getTruePos(const sf::Vector2f pos_) const;
  void zoomOnPoint(float factor, sf::Vector2f pos_);
  void handleEvent(const sf::Event event);
  const float getScale() const;
  const sf::Vector2f getPos() const;
  const sf::Vector2f getRelativeMousePos() const;
  const bool getMouseDown() const;

private:
  std::vector<sf::RenderStates *> states_vector;
  sf::Transform transform;
  sf::Vector2f pos;
  sf::Vector2f anchor_pos;
  bool mouse_down;
  float scale;
  static constexpr float SCROLL_ZOOM_MAG = 0.8;
  sf::Vector2f mouse_pos;
};
