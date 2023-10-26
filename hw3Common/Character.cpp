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

GameObject::ObjectStruct *Character::toStruct() {
    CharStruct *rtn = new CharStruct;
    rtn->type = 1;
    rtn->posX = getPosition().x;
    rtn->posY = getPosition().y;
    rtn->r = getFillColor().r;
    rtn->g = getFillColor().g;
    rtn->b = getFillColor().b;
    return rtn;
}

GameObject *Character::constructSelf(GameObject::ObjectStruct *self) {
    Character rtn;
    CharStruct* realSelf = (CharStruct*)self;
    if (self->type != 1) {
        throw std::invalid_argument("ObjectStruct must be of type Character");
    }
    rtn.setPosition(realSelf->x, realSelf->y);
    

}

GameObject *Character::makeTemplate() {
    return new Character;
}



