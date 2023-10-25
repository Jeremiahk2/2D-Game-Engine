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

