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
void GameWindow::addCharacter(Character* character) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    this->character = character;
}

Character* GameWindow::getCharacter() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return character;
}

void GameWindow::addPlatform(Platform* platform, bool isMoving) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    //Add the platform to the list of platforms
    platforms.push_front(platform);
    //Make a collision box for the platform and add it in.
    collisions.push_front(CBox(isMoving, platform));
    //If it is a moving platform, add it to the list of moving platforms.
    if (isMoving) {
        movings.push_front((MovingPlatform*)platform);
    }
    numPlatforms++;
}

Platform* GameWindow::getPlatforms(int* n) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    Platform rtnPlatforms[10];
    int count = 0;
    for (Platform* i : platforms) {
        rtnPlatforms[count] = *i;
        count++;
    }
    *n = count;
    return rtnPlatforms;
}

list<MovingPlatform*>* GameWindow::getMovings() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return &movings;
}

void GameWindow::update() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    clear();
    //Cycle through the list of platforms and draw them.
    for (Platform* i : platforms) {
        draw(*i);
    }

    for (auto iter = characters->begin(); iter != characters->end(); ++iter) {
        draw(iter->second);
    }
    display();
}

void GameWindow::updateCharacters(char *newChars) {
    std::lock_guard<std::mutex> lock(*innerMutex);
    int currentId = 0;
    float currentX = 0.f;
    float currentY = 0.f;

    int count = 0;
    int pos = 0;
    int newPos = 0;
    //Parse the string to find the information about each character
    characters->clear();
    while (sscanf_s(newChars + pos, "%d %f %f %n", &currentId, &currentX, &currentY, &newPos) == 3) {
        templateCharacter.setID(currentId);
        templateCharacter.setPosition(currentX, currentY);
        characters->insert_or_assign(currentId, templateCharacter);
    }
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