#include "Platform.h"

Platform::Platform() : sf::RectangleShape() {
    passthrough = false;

    mutex = (std::mutex*)malloc(sizeof(std::mutex));
    std::mutex tempMutex;
    if (mutex != NULL) {
        memcpy(mutex, &tempMutex, sizeof(std::mutex));
    }
}

void Platform::setPassthrough(bool passthrough) {
    this->passthrough = passthrough;
}

void Platform::move(float offsetX, float offsetY) {
    std::lock_guard<std::mutex> lock(*mutex);
    sf::RectangleShape::move(offsetX, offsetY);
}

void Platform::move(const sf::Vector2f offset) {
    std::lock_guard<std::mutex> lock(*mutex);
    sf::RectangleShape::move(offset);
}

void Platform::setPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(*mutex);
    sf::RectangleShape::setPosition(x, y);
}

void Platform::setPosition(const sf::Vector2f position) {
    std::lock_guard<std::mutex> lock(*mutex);
    sf::RectangleShape::setPosition(position);
}

sf::FloatRect Platform::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(*mutex);
    return sf::RectangleShape::getGlobalBounds();
}

std::mutex* Platform::getMutex() {
    return mutex;
}