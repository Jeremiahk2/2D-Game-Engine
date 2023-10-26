#include "GameWindow.h"

GameWindow::GameWindow() {
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }

    innerMutex = (std::mutex*)malloc(sizeof(std::mutex));
    std::mutex tempMutex;
    if (innerMutex != NULL) {
        memcpy(innerMutex, &tempMutex, sizeof(std::mutex));
    }

    templateCharacter.setSize(sf::Vector2f(30.f, 30.f));
    templateCharacter.setFillColor(sf::Color::White);
    templateCharacter.setOrigin(0.f, 30.f);
    templateCharacter.setPosition(0, 0);
    templateCharacter.setTexture(&charTexture);

    character = NULL;
}

bool GameWindow::checkCollisions(CBox* collides) {
    bool foundCollision = false;
    CBox collidable = *collides;
    //Cycle through the list of collidables and check if they collide with the player.
    {
        std::lock_guard<std::mutex> lock(*innerMutex);
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
    }
    // If the only found collision was moving, return it.
    if (foundCollision) {
        *collides = collidable;
        return true;
    }
    // No collision found.
    return false;
}

//Add this client's playable character.
void GameWindow::addPlayableObject(GameObject* character) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    this->character = character;
}

GameObject* GameWindow::getPlayableObject() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return character;
}

void GameWindow::addGameObject(GameObject *object) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    if (object->isStatic()) {
        staticObjects.push_back(object);
    }
    else {
        nonStaticObjects.push_back(object);
    }
    if (object->isCollidable()) {
        collisions.push_front(CBox(object));
    }
}
//Contains client objects like death bounds and side bounds.
list<GameObject*>* GameWindow::getStaticObjects() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return &staticObjects;
}
//Contains server objects like moving platforms and other characters.
list<GameObject*>* GameWindow::getNonstaticObjects() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return &nonStaticObjects;
}

void GameWindow::update() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    clear();
    //Cycle through the list of platforms and draw them.
    for (GameObject* i : staticObjects) {
        draw(*i);
    }
    for (GameObject* i : nonStaticObjects) {
        draw(*i);
    }
    display();
}

void GameWindow::addTemplate(std::unique_ptr<GameObject> templateObject) {

}

void GameWindow::updateNonStatic(GameObject::ObjectStruct * objects) {
    std::lock_guard<std::mutex> lock(*innerMutex);

    
}

void GameWindow::changeScaling() {
    isProportional = !isProportional;
}

bool GameWindow::checkMode() {
    return isProportional;
}

void GameWindow::handleResize(sf::Event event) {
    if (!isProportional) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        setView(sf::View(visibleArea));
    }
}