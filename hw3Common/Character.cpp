#include "Character.h"

Character::Character() : GameObject(false, false, true), Sprite() {
    /**
    ART FOR SANTA PROVIDED BY Marco Giorgini THROUGH OPENGAMEART.ORG
    License: CC0 (Public Domain) @ 12/28/21
    https://opengameart.org/content/santas-rats-issue-c64-sprites
    */
    sf::Texture charTexture;
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
    setTexture(charTexture);
    /*setSize(sf::Vector2f(30.f, 30.f));*/
    setOrigin(0.f, 30.f);
    setSpeed(CHAR_SPEED);
    setGravity(GRAV_SPEED);
}

Character::Character(bool stationary, bool collidable, bool drawable) : GameObject(stationary, collidable, drawable) {
    /**
    ART FOR SANTA PROVIDED BY Marco Giorgini THROUGH OPENGAMEART.ORG
    License: CC0 (Public Domain) @ 12/28/21
    https://opengameart.org/content/santas-rats-issue-c64-sprites
    */
    sf::Texture charTexture;
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
    setTexture(charTexture);
    /*setSize(sf::Vector2f(30.f, 30.f));*/
    setOrigin(0.f, 30.f);
    setSpeed(CHAR_SPEED);
    setGravity(GRAV_SPEED);
}

bool Character::isConnecting() {
    return connecting;
}

void Character::setConnecting(bool connecting) {
    this->connecting = connecting;
}

void Character::move(float offsetX, float offsetY) {
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::move(offsetX, offsetY);
}

void Character::move(const sf::Vector2f offset) {
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::move(offset);
}

void Character::setPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::setPosition(x, y);
}

void Character::setPosition(const sf::Vector2f position) {
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::setPosition(position);
}

sf::Vector2f Character::getPosition() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::Sprite::getPosition();
}

sf::FloatRect Character::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::Sprite::getGlobalBounds();
}

sf::Vector2f Character::getSpeed() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return speed;
}

void Character::setSpeed(float speed) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->speed.x = speed;
}

bool Character::isJumping() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return jumping;
}

void Character::setJumping(bool jump) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->jumping = jump;
}

float Character::getGravity() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return gravity;
}

void Character::setGravity(float gravity) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->gravity = gravity;
}

void Character::setID(int id) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->id = id;
}

int Character::getID() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return id;
}

int Character::getObjectType() {
    return this->objectType;
}

std::string Character::toString()
{
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << isConnecting() << space << getID() << space << getPosition().x << space << getPosition().y;
    std::string line;
    std::getline(stream, line);
    return line;
}

std::shared_ptr<GameObject> Character::constructSelf(std::string self)
{

    Character *c = new Character;
    int type;
    bool connecting;
    int id;
    float x;
    float y;

    int matches = sscanf(self.data(), "%d, %c %d %f %f", &type, &connecting, &id, &x, &y);
    if (matches != 5 || type != getObjectType()) {
        throw std::invalid_argument("Type was not correct for character or string was formatted wrong.");
    }


    c->setPosition(x, y);
    c->setConnecting(connecting);
    c->setID(id);

    GameObject* go = (GameObject*)c;
    std::shared_ptr<GameObject> ptr(go);
    return ptr;
}

std::shared_ptr<GameObject> Character::makeTemplate()
{
    std::shared_ptr<GameObject> ptr(new Character);
    return ptr;
}