#include "ViewPort.hpp"

ViewPort::ViewPort(std::vector<sf::RenderStates*> statesV_, sf::Vector2f pos_, float zoom_) :
    states_vector(statesV_),
    pos(0,0),
	anchor_pos(0,0),
	mouse_down(false),
	scale(1.0f),
	mouse_pos(0,0)
{
    zoom(zoom_);
    move(pos_);
};

void ViewPort::updateStates(sf::Transform& transform) {
    for (sf::RenderStates* state : states_vector) {
        state->transform = transform;
    }
}

void ViewPort::zoom(float factor) {
    transform.scale(factor, factor);
    scale *= factor;
    updateStates(transform);
}

void ViewPort::move(sf::Vector2f pos_) {
    transform.translate(pos_ / scale);
    pos += pos_;
    updateStates(transform);
}

void ViewPort::moveTo(sf::Vector2f pos_) {
    move(pos_ - pos);
}

sf::Vector2f ViewPort::getTruePos(sf::Vector2f pos_) const {
    return (pos_ - pos) / scale;
}

void ViewPort::zoomOnPoint(float factor, sf::Vector2f pos_) {
    const float rec = 1.0f - factor;
    sf::Vector2f truePos = getTruePos(pos_);
    move((truePos * rec) * scale);
    zoom(factor);
}

void ViewPort::handleEvent(sf::Event event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta < 0) {
            zoomOnPoint(SCROLL_ZOOM_MAG, mouse_pos);
        }
        else {
            zoomOnPoint(1.0f / SCROLL_ZOOM_MAG, mouse_pos);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f truePos = getTruePos(mouse_pos);
        anchor_pos = truePos;
        mouse_down = true;
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        mouse_down = false;
    }
    else if (event.type == sf::Event::MouseMoved) {
        mouse_pos.x = static_cast<float>(event.mouseMove.x);
        mouse_pos.y = static_cast<float>(event.mouseMove.y);
    }
    if (mouse_down) {
        sf::Vector2f truePos = getTruePos(mouse_pos);
        move((truePos - anchor_pos) * scale);
    }
}

float ViewPort::getScale() const {
    return scale;
}

sf::Vector2f ViewPort::getPos() const {
    return pos;
}