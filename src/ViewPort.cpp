#include "ViewPort.hpp"

ViewPort::ViewPort(std::vector<sf::RenderStates*> statesV_, sf::Vector2f pos_, float zoom_) :
    statesV(statesV_),
    pos(0,0),
	anchorPos(0,0),
	mouseDown(false),
	scale(1.0f),
	mousePos(0,0)
{
    zoom(zoom_);
    move(pos_);
};

void ViewPort::updateStates(sf::Transform& transform) {
    for (sf::RenderStates* state : statesV) {
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
            zoomOnPoint(scrollZoomMag, mousePos);
        }
        else {
            zoomOnPoint(1.0f / scrollZoomMag, mousePos);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f truePos = getTruePos(mousePos);
        anchorPos = truePos;
        mouseDown = true;
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        mouseDown = false;
    }
    else if (event.type == sf::Event::MouseMoved) {
        mousePos.x = static_cast<float>(event.mouseMove.x);
        mousePos.y = static_cast<float>(event.mouseMove.y);
    }
    if (mouseDown) {
        sf::Vector2f truePos = getTruePos(mousePos);
        move((truePos - anchorPos) * scale);
    }
}

float ViewPort::getScale() const {
    return scale;
}

sf::Vector2f ViewPort::getPos() const {
    return pos;
}