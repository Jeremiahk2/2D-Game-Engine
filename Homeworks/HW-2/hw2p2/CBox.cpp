#include "CBox.h"


bool movable = false;
Platform *platform;

CBox::CBox() {}

CBox::CBox(bool movable, Platform *platform) {
    this->movable = movable;
    this->platform = platform;
}

bool CBox::isMoving() {
    return movable;
}

sf::FloatRect CBox::getCBox() {
    return platform->getGlobalBounds();
}

Platform *CBox::getPlatform() {
    return platform;
}