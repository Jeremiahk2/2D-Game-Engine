#include "GameWindow.h"
//Correct version

list<Platform*> platforms;

Character* character;

list<CBox> collisions;

bool isProportional = true;

int numPlatforms = 0;

Character allCharacters[10];

int numCharacters = 0;

Platform realPlatforms[10];

sf::Texture charTexture;


GameWindow::GameWindow() {
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
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
    for (int i = 0; i < numCharacters; i++) {
        if ((allCharacters[i].getID() >= 0) ) {
            draw(allCharacters[i]);
        }
    }
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
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
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

void GameWindow::updateCharacters(char *newChars) {
    
    int currentId = 0;
    float currentX = 0.f;
    float currentY = 0.f;

    int count = 0;
    int pos = 0;
    int newPos = 0;

    while (sscanf_s(newChars + pos, "%d %f %f %n", &currentId, &currentX, &currentY, &newPos) == 3) {
        pos += newPos;
        //If it is one of the already registered characters, update it.
        if (count < numCharacters) {
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

            allCharacters[count] = newCharacter;
            numCharacters++;
        }
        count++;
    }
    

}