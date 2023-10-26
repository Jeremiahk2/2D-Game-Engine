#include "Character.h"

Character::Character() : GameObject(true, true) {
    /**
    ART FOR SANTA PROVIDED BY Marco Giorgini THROUGH OPENGAMEART.ORG
    License: CC0 (Public Domain) @ 12/28/21
    https://opengameart.org/content/santas-rats-issue-c64-sprites
    */
    sf::Texture charTexture;
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
    setTexture(&charTexture);
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

int Character::getType() {
    return this->type;
}

GameObject::ObjectStruct Character::toStruct() {
    Character::CharStruct character;
    character.posX = getPosition().x;
    character.posY = getPosition().y;
    character.sizeX = getSize().x;
    character.sizeY = getSize().y;
    character.r = getFillColor().r;
    character.g = getFillColor().g;
    character.b = getFillColor().b;
    character.type = getType();
    //character.size? To tell it the size of the struct?
    return character;
    //Needs to be a pointer to a struct
}

std::shared_ptr<GameObject> Character::constructSelf(GameObject::ObjectStruct self) {
    Character character;
    if (type != getType()) {
        throw std::invalid_argument("ObjectStruct must be a CharStruct");
    }
    CharStruct s = (CharStruct)self;

    character.setPosition(self.posX);
    //Needs to be a pointer to a struct.
}

std::shared_ptr<GameObject> Character::makeTemplate() {

}



