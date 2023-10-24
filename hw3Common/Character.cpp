#include "Character.h"

Character::Character() {
    innerMutex = (std::mutex *)malloc(sizeof(std::mutex));
    std::mutex tempMutex;
    if (innerMutex != NULL) {
        memcpy(innerMutex, &tempMutex, sizeof(std::mutex));
    }
}

sf::Vector2f Character::getSpeed() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return speed;
}

void Character::setSpeed(float speed) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    this->speed.x = speed;
}

bool Character::isJumping() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return jumping;
}

void Character::setJumping(bool jump) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    this->jumping = jump;
}

float Character::getGravity() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return gravity;
}

void Character::setGravity(float gravity) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    this->gravity = gravity;
}

void Character::setID(int id) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    this->id = id;
}

int Character::getID() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return id;
}

void Character::move(float offsetX, float offsetY) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::move(offsetX, offsetY);
}

void Character::move(const sf::Vector2f offset) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::move(offset);
}

void Character::setPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::setPosition(x, y);
}

void Character::setPosition(const sf::Vector2f position) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::setPosition(position);
}

sf::Vector2f Character::getPosition() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return sf::RectangleShape::getPosition();
}

sf::FloatRect Character::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return sf::RectangleShape::getGlobalBounds();
}

void Character::to_json(json& j, const GameObject& g) {

}

void Character::from_json(const json& j, GameObject& g) {

}
