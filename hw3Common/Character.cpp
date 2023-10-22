#include "Character.h"
#define DEFAULT_SPEED 1.f

sf::Vector2f speed = sf::Vector2f(DEFAULT_SPEED, 0);

bool jumping = false;

float gravity = 0.f;

int id = -1;

std::mutex *charMutex;

Character::Character() {
    charMutex = (std::mutex *)malloc(sizeof(std::mutex));

    std::mutex tempMutex;
    if (charMutex != NULL) {
        memcpy(charMutex, &tempMutex, sizeof(std::mutex));
    }
}

sf::Vector2f Character::getSpeed() {
    std::lock_guard<std::mutex> lock(*charMutex);
    return speed;
}

void Character::setSpeed(float speed) {
    std::lock_guard<std::mutex> lock(*charMutex);
    this->speed.x = speed;
}

bool Character::isJumping() {
    std::lock_guard<std::mutex> lock(*charMutex);
    return jumping;
}

void Character::setJumping(bool jump) {
    std::lock_guard<std::mutex> lock(*charMutex);
    this->jumping = jump;
}

float Character::getGravity() {
    std::lock_guard<std::mutex> lock(*charMutex);
    return gravity;
}

void Character::setGravity(float gravity) {
    std::lock_guard<std::mutex> lock(*charMutex);
    this->gravity = gravity;
}

void Character::setID(int id) {
    std::lock_guard<std::mutex> lock(*charMutex);
    this->id = id;
}

int Character::getID() {
    std::lock_guard<std::mutex> lock(*charMutex);
    return id;
}

void Character::move(float offsetX, float offsetY) {
    std::lock_guard<std::mutex> lock(*charMutex);
    sf::RectangleShape::move(offsetX, offsetY);
}

void Character::move(const sf::Vector2f offset) {
    std::lock_guard<std::mutex> lock(*charMutex);
    sf::RectangleShape::move(offset);
}

void Character::setPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(*charMutex);
    sf::RectangleShape::setPosition(x, y);
}

void Character::setPosition(const sf::Vector2f position) {
    std::lock_guard<std::mutex> lock(*charMutex);
    sf::RectangleShape::setPosition(position);
}

sf::FloatRect Character::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(*charMutex);
    return sf::RectangleShape::getGlobalBounds();
}