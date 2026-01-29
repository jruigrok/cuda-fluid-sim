#include "ViewPort.hpp"

ViewPort::ViewPort(const std::vector<sf::RenderStates *> statesV_,
                   const sf::Vector2f pos_, const float zoom_)
    : states_vector(statesV_), pos(0, 0), anchor_pos(0, 0), mouse_down(false),
      scale(1.0f), mouse_pos(0, 0) {
  zoom(zoom_);
  move(pos_);
};

void ViewPort::updateStates(const sf::Transform &transform) {
  for (sf::RenderStates *state : states_vector) {
    state->transform = transform;
  }
}

void ViewPort::zoom(const float factor) {
  transform.scale(factor, factor);
  scale *= factor;
  updateStates(transform);
}

void ViewPort::move(const sf::Vector2f pos_) {
  transform.translate(pos_ / scale);
  pos += pos_;
  updateStates(transform);
}

void ViewPort::moveTo(const sf::Vector2f pos_) { move(pos_ - pos); }

const sf::Vector2f ViewPort::getTruePos(const sf::Vector2f pos_) const {
  return (pos_ - pos) / scale;
}

const sf::Vector2f ViewPort::getRelativeMousePos() const {
  return getTruePos(mouse_pos);
}

const bool ViewPort::getMouseDown() const { return mouse_down; }

void ViewPort::zoomOnPoint(float factor, sf::Vector2f pos_) {
  const float rec = 1.0f - factor;
  sf::Vector2f truePos = getTruePos(pos_);
  move((truePos * rec) * scale);
  zoom(factor);
}

void ViewPort::handleEvent(const sf::Event event) {
  if (event.type == sf::Event::MouseWheelScrolled) {
    if (event.mouseWheelScroll.delta < 0) {
      zoomOnPoint(SCROLL_ZOOM_MAG, mouse_pos);
    } else {
      zoomOnPoint(1.0f / SCROLL_ZOOM_MAG, mouse_pos);
    }
  } else if (event.type == sf::Event::MouseButtonPressed) {
    sf::Vector2f truePos = getTruePos(mouse_pos);
    anchor_pos = truePos;
    mouse_down = true;
  } else if (event.type == sf::Event::MouseButtonReleased) {
    mouse_down = false;
  } else if (event.type == sf::Event::MouseMoved) {
    mouse_pos.x = static_cast<float>(event.mouseMove.x);
    mouse_pos.y = static_cast<float>(event.mouseMove.y);
  }
  if (mouse_down) {
    sf::Vector2f truePos = getTruePos(mouse_pos);
    move((truePos - anchor_pos) * scale);
  }
}

const float ViewPort::getScale() const { return scale; }

const sf::Vector2f ViewPort::getPos() const { return pos; }