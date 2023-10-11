#include "GameWindow.h"

list<Platform*> platforms;

Character *character;

list<CBox> collisions;

bool isProportional = true;


GameWindow::GameWindow() {
}

void GameWindow::addPlatform(Platform *platform, bool isMoving) {
    platforms.push_front(platform);
    collisions.push_front(CBox(isMoving, platform));
}

void GameWindow::update() {
    clear();
    //Cycle through the list of platforms and draw them.
    for (Platform *i : platforms) {
        draw(*i);
    }
    draw(*character);
    display();
}

void GameWindow::addCharacter(Character *character) {
    this->character = character;
}

bool GameWindow::checkCollisions(CBox *collides) {
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

Character *GameWindow::getCharacter() {
    return character;
}