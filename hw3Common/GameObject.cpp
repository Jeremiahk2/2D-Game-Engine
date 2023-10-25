#include "GameObject.h"

GameObject::GameObject(bool stationary, bool collidable)
{
    innerMutex = (std::mutex*)malloc(sizeof(std::mutex));
    std::mutex tempMutex;
    if (innerMutex != NULL) {
        memcpy(innerMutex, &tempMutex, sizeof(std::mutex));
    }
    this->stationary = stationary;
    this->collidable = collidable;
}

void GameObject::move(float offsetX, float offsetY) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::move(offsetX, offsetY);
}

void GameObject::move(const sf::Vector2f offset) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::move(offset);
}

void GameObject::setPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::setPosition(x, y);
}

void GameObject::setPosition(const sf::Vector2f position) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    sf::RectangleShape::setPosition(position);
}

 sf::Vector2f GameObject::getPosition() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return sf::RectangleShape::getPosition();
}

sf::FloatRect GameObject::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return sf::RectangleShape::getGlobalBounds();
}

bool GameObject::isStatic() {
    return stationary;
}

std::mutex* GameObject::getMutex() {
    return innerMutex;
}

bool GameObject::isCollidable()
{
    return collidable;
}

void GameObject::setCollidable(bool collidable)
{
    this->collidable = collidable;
}

std::string GameObject::toString() {
    std::string rtnString = "Object " + std::to_string(getPosition().x) + " " + std::to_string(getPosition().y);
    return rtnString;
}

void GameObject::constructSelf(std::string) {

}
