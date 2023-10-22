#include "GameWindow.h"

list<Platform*> platforms;
list<CBox> collisions;
list<MovingPlatform*> movings;

Character allCharacters[10];
Platform realPlatforms[10];

Character* character;
sf::Texture charTexture;

bool isProportional = true;
int numPlatforms = 0;
int numCharacters = 0;

std::mutex* windowMutex;




GameWindow::GameWindow() {
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }

    windowMutex = (std::mutex*)malloc(sizeof(std::mutex));

    std::mutex tempMutex;
    if (windowMutex != NULL) {
        memcpy(windowMutex, &tempMutex, sizeof(std::mutex));
    }
}

bool GameWindow::checkCollisions(CBox* collides) {
    bool foundCollision = false;
    CBox collidable = *collides;
    //Cycle through the list of collidables and check if they collide with the player.
    {
        std::lock_guard<std::mutex> lock(*windowMutex);
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
    std::lock_guard<std::mutex> lock(*windowMutex);
    this->character = character;
}

Character* GameWindow::getCharacter() {
    std::lock_guard<std::mutex> lock(*windowMutex);
    return character;
}

void GameWindow::addPlatform(Platform* platform, bool isMoving) {
    std::lock_guard<std::mutex> lock(*windowMutex);
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
    std::lock_guard<std::mutex> lock(*windowMutex);
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
    std::lock_guard<std::mutex> lock(*windowMutex);
    return &movings;
}

void GameWindow::update() {
    std::lock_guard<std::mutex> lock(*windowMutex);
    clear();
    //Cycle through the list of platforms and draw them.
    for (Platform* i : platforms) {
        draw(*i);
    }
    //Cycle through the list of characters and draw them (Our character is included here)
    for (int i = 0; i < numCharacters; i++) {
        if ((allCharacters[i].getID() >= 0)) {
            draw(allCharacters[i]);
        }
    }
    display();
}

void GameWindow::updateCharacters(char *newChars) {
    
    int currentId = 0;
    float currentX = 0.f;
    float currentY = 0.f;

    int count = 0;
    int pos = 0;
    int newPos = 0;
    //Parse the string to find the information about each character
    while (sscanf_s(newChars + pos, "%d %f %f %n", &currentId, &currentX, &currentY, &newPos) == 3) {
        pos += newPos;
        //If it is one of the already registered characters, update it.
        if (count < numCharacters) {
            std::lock_guard<std::mutex> lock(*windowMutex);
            allCharacters[count].setID(currentId);
            allCharacters[count].setPosition(currentX, currentY);
        }
        //If the server added a new character, create it and store it.
        else {
            Character newCharacter;
            newCharacter.setSize(sf::Vector2f(30.f, 30.f));
            newCharacter.setFillColor(sf::Color::White);
            newCharacter.setOrigin(0.f, 30.f);
            newCharacter.setID(currentId);
            newCharacter.setPosition(currentX, currentY);
            newCharacter.setTexture(&charTexture);
            {
                std::lock_guard<std::mutex> lock(*windowMutex);
                allCharacters[count] = newCharacter;
                numCharacters++;
            }
        }
        count++;
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