#include "Platform.h"


bool passthrough;

std::mutex *platMutex;



Platform::Platform() : sf::RectangleShape() {
    passthrough = false;

    platMutex = (std::mutex*)malloc(sizeof(std::mutex));
    std::mutex tempMutex;
    if (platMutex != NULL) {
        memcpy(platMutex, &tempMutex, sizeof(std::mutex));
    }
}

void Platform::setPassthrough(bool passthrough) {
    this->passthrough = passthrough;
}

void Platform::move(float offsetX, float offsetY) {
    std::lock_guard<std::mutex> lock(*platMutex);
    sf::RectangleShape::move(offsetX, offsetY);
}

void Platform::move(const sf::Vector2f offset) {
    std::lock_guard<std::mutex> lock(*platMutex);
    sf::RectangleShape::move(offset);
}

void Platform::setPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(*platMutex);
    sf::RectangleShape::setPosition(x, y);
}

void Platform::setPosition(const sf::Vector2f position) {
    std::lock_guard<std::mutex> lock(*platMutex);
    sf::RectangleShape::setPosition(position);
}

sf::FloatRect Platform::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(*platMutex);
    return sf::RectangleShape::getGlobalBounds();
}

std::mutex* Platform::getMutex() {
    return platMutex;
}