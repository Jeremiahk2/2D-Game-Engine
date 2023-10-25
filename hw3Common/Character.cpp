#include "Character.h"

Character::Character() : GameObject(true) {
    
}

sf::Vector2f Character::getSpeed() {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    return speed;
}

void Character::setSpeed(float speed) {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    this->speed.x = speed;
}

bool Character::isJumping() {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    return jumping;
}

void Character::setJumping(bool jump) {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    this->jumping = jump;
}

float Character::getGravity() {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    return gravity;
}

void Character::setGravity(float gravity) {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    this->gravity = gravity;
}

void Character::setID(int id) {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    this->id = id;
}

int Character::getID() {
    std::lock_guard<std::mutex> lock(*(getMutex()));
    return id;
}

std::string Character::toString()
{
    std::string rtnString = "Character " + std::to_string(getID()) + " " + std::to_string(getPosition().x) + " " + std::to_string(getPosition().y);
    return rtnString;
}



