#include "GameWindow.h"

list<Platform*> platforms;

Character* character;

list<CBox> collisions;

bool isProportional = true;

int numPlatforms = 0;

list<Character> allCharacters;

Platform realPlatforms[10];


GameWindow::GameWindow() {
}

void GameWindow::addPlatform(Platform* platform, bool isMoving) {
    platforms.push_front(platform);
    collisions.push_front(CBox(isMoving, platform));
    numPlatforms++;
}

void GameWindow::update() {
    clear();
    //Cycle through the list of platforms and draw them.
    for (Platform* i : platforms) {
        draw(*i);
    }
    for (Character i : allCharacters) {
        draw(i);
    };
    display();
}

void GameWindow::addCharacter(Character* character) {
    this->character = character;
}

bool GameWindow::checkCollisions(CBox* collides) {
    bool foundCollision = false;
    CBox collidable = *collides;
    //Cycle through the list of collidables and check if they collide with the player.
    for (CBox i : collisions) {
        if (character->getGlobalBounds().intersects(i.getCBox())) {
            foundCollision = true;
            collidable = i;
            // If the found collision is not moving, return it immediately
            if (!i.isMoving()) {
                *collides = i;
                return true;
            }
        }
    }
    // If the only found collision was moving, return it.
    if (foundCollision) {
        *collides = collidable;
        return true;
    }
    // No collision found.
    return false;
}

void GameWindow::changeScaling() {
    isProportional = !isProportional;
}

bool GameWindow::checkMode() {
    return isProportional;
}

void GameWindow::handleResize(sf::Event event) {
    if (!isProportional) {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        setView(sf::View(visibleArea));
    }
}

Character* GameWindow::getCharacter() {
    return character;
}

Platform* GameWindow::getPlatforms(int* n) {
    Platform rtnPlatforms[10]; //Update this later to not be capped at 10
    int count = 0;
    for (Platform* i : platforms) {
        rtnPlatforms[count] = *i;
        count++;
    }
    *n = count;
    return rtnPlatforms;
}

void GameWindow::updatePlatforms(Platform* newPlatforms, int n) {
    platforms.clear();
    for (int i = 0; i < n; i++) {
        Platform newPlatform;
        newPlatform.setOrigin(newPlatforms->getOrigin());
        newPlatform.setPosition(newPlatforms->getPosition());
        newPlatform.setSize(newPlatforms->getSize());
        newPlatform.setFillColor(newPlatforms->getFillColor());
        realPlatforms[i] = newPlatform;
        platforms.push_front(&realPlatforms[i]);
    }
}

void GameWindow::updateCharacters(Character* otherCharacters, int n) {
    allCharacters.clear();
    for (int i = 0; i < n; i++) {
        Character newCharacter;
        newCharacter.setOrigin(otherCharacters[i].getOrigin());
        //newCharacter.setTexture(otherCharacters[i].getTexture());
        newCharacter.setSize(otherCharacters[i].getSize());
        newCharacter.setPosition(otherCharacters[i].getPosition());
        newCharacter.setFillColor(otherCharacters[i].getFillColor());
        allCharacters.push_front(newCharacter);
    }
}