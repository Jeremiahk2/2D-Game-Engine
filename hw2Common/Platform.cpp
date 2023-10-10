#include "Platform.h"
//Correct version


bool passthrough;



Platform::Platform() : sf::RectangleShape() {
    passthrough = false;
}

void Platform::setPassthrough(bool passthrough) {
    this->passthrough = passthrough;
}