#include "Character.h"
#define DEFAULT_SPEED 1.f

sf::Vector2f speed = sf::Vector2f(DEFAULT_SPEED, 0);

bool jumping = false;

double gravity = 0.f;

int id = -1;

Character::Character() {
}
    
sf::Vector2f Character::getSpeed() {
    return speed;
}
    
void Character::setSpeed(double speed) {
    this->speed.x = speed;
}

bool Character::isJumping() {
    return jumping;
}

void Character::setJumping(bool jump) {
    this->jumping = jump;
}

double Character::getGravity() {
    return gravity;
}

void Character::setGravity(double gravity) {
    this->gravity = gravity;
}

void Character::setID(int id) {
    this->id = id;
}

int Character::getID() {
    return id;
}