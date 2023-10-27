#include "Platform.h"

Platform::Platform() : sf::RectangleShape(), GameObject(true, true, true) {
    passthrough = false;
}

Platform::Platform(bool stationary, bool collidable, bool drawable) : GameObject(stationary, collidable, drawable)
{
    passthrough = false;
}

void Platform::setPassthrough(bool passthrough) {
    this->passthrough = passthrough;
}

void Platform::move(float offsetX, float offsetY) {
    float roundedX = std::roundf(100 * offsetX) / 100.f;
    float roundedY = std::roundf(100 * offsetY) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::move(roundedX, roundedY);
}

void Platform::move(const sf::Vector2f offset) {
    float roundedX = std::roundf(100 * offset.x) / 100.f;
    float roundedY = std::roundf(100 * offset.y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::move(roundedX, roundedY);
}

void Platform::setPosition(float x, float y) {
    float roundedX = std::roundf(100 * x) / 100.f;
    float roundedY = std::roundf(100 * y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::setPosition(roundedX, roundedY);
}

void Platform::setPosition(const sf::Vector2f position) {
    float roundedX = std::roundf(100 * position.x) / 100.f;
    float roundedY = std::roundf(100 * position.y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::setPosition(roundedX, roundedY);
}

sf::Vector2f Platform::getPosition() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::RectangleShape::getPosition();
}

sf::FloatRect Platform::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::RectangleShape::getGlobalBounds();
}

std::mutex* Platform::getMutex() {
    return &innerMutex;
}

/**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
std::string Platform::toString() {
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y
        << space << getFillColor().r << space << getFillColor().g << space << getFillColor().b;
    std::string line;
    std::getline(stream, line);
    return line;
}

/**
* Creates a new object using the string provided. To work with GameWindow, this should match toString.
*/
std::shared_ptr<GameObject> Platform::constructSelf(std::string self) {
    Platform* c = new Platform;
    int type;
    float x;
    float y;
    float sizeX;
    float sizeY;
    int r;
    int g;
    int b;
    int matches = sscanf_s(self.data(), "%d %f %f %f %f %d %d %d", &type, &x, &y, &sizeX, &sizeY, &r, &g, &b);
    if (matches != 8 || type != getObjectType()) {
        throw std::invalid_argument("Type was not correct for character or string was formatted wrong.");
    }

    c->setPosition(x, y);
    sf::Vector2f size(sizeX, sizeY);
    c->setSize(size);
    sf::Color color(r, g, b);
    c->setFillColor(color);

    GameObject* go = (GameObject*)c;
    std::shared_ptr<GameObject> ptr(go);
    return ptr;
}

/**
* Creates a blank GameObject. Useful for when you need an instance of the object without knowing what type it is.
*/
std::shared_ptr<GameObject> Platform::makeTemplate() {
    std::shared_ptr<GameObject> ptr(new Platform);
    return ptr;
}

/**
* Return the type of the class. Type should be a static const variable so that it is constant across all versions.
* type should be unique for each version of GameObject that you use with GameWindow.
*/
int Platform::getObjectType() {
    return this->objectType;
}