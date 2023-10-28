#include "GameWindow.h"

GameWindow::GameWindow() {

    innerMutex = (std::mutex*)malloc(sizeof(std::mutex));
    std::mutex tempMutex;
    if (innerMutex != NULL) {
        memcpy(innerMutex, &tempMutex, sizeof(std::mutex));
    }

    character = NULL;
}

bool GameWindow::checkCollisions(GameObject* collides) {
    bool foundCollision = false;
    //Cycle through the list of collidables and check if they collide with the player.
    {
        std::lock_guard<std::mutex> lock(*innerMutex);
        for (GameObject *i : collidables) {

            if (((sf::Sprite *)character)->getGlobalBounds().intersects(((sf::Shape *)i)->getGlobalBounds())) {
                // If the found collision is not moving, return it immediately
                collides = i;
                return true;
            }
        }
        for (std::shared_ptr<GameObject> i : nonStaticObjects) {
            if (i->isCollidable() && ((sf::Sprite*)character)->getGlobalBounds().intersects(((sf::Shape*)i.get())->getGlobalBounds())) {
                // If the found collision is not moving, return it immediately
                if (i->isStatic()) {
                    collides = i.get();
                    return true;
                }
            }
        }

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
    if (object->isCollidable()) {
        collidables.push_front(object);
    }
    if (object->isDrawable()) {
        drawables.push_front(object);
    }
}
//Contains client objects like death bounds and side bounds.
std::list<GameObject*>* GameWindow::getStaticObjects() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return &staticObjects;
}
//Contains server objects like moving platforms and other characters.
std::list<std::shared_ptr<GameObject>>* GameWindow::getNonstaticObjects() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    return &nonStaticObjects;
}

void GameWindow::update() {
    std::lock_guard<std::mutex> lock(*innerMutex);
    clear();
    //Cycle through the list of platforms and draw them.
    for (GameObject* i : drawables) {
        std::cout << "Drawing..." << std::endl;
        draw(*(dynamic_cast<sf::Drawable*>(i))); //FUCK YES
    }
    for (std::shared_ptr<GameObject> i : nonStaticObjects) {
        if (i->isDrawable()) {
            draw(*((sf::Drawable*)i.get()));
        }
    }
    nonStaticObjects.clear();
    display();
}

void GameWindow::addTemplate(std::shared_ptr<GameObject> templateObject) {
    templates.insert_or_assign(templateObject->getObjectType(), templateObject);
}

void GameWindow::updateNonStatic(std::string updates) {
    std::lock_guard<std::mutex> lock(*innerMutex);

    char* currentObject = (char*)malloc(updates.size() + 1);
    if (currentObject == NULL) {
        throw std::runtime_error("Memory error while updating static objects");
    }
    int pos = 0;
    int newPos = 0;

    //Scan through each object
    while (sscanf_s(updates.data() + pos, "%[^,]%n", currentObject,(unsigned int)(updates.size() + 1), &newPos) == 1) {
        //Get the type of the current object.
        int type;
        int matches = sscanf_s(currentObject, "%d", &type);
        if (matches != 1) {
            free(currentObject);
            throw std::invalid_argument("Failed to read string. Type must be the first value.");
        }
        //Push the newly created object into the array.
        std::cout << "Ran" << std::endl;
        std::cout << currentObject << std::endl;
        nonStaticObjects.push_back(templates.at(type)->constructSelf(currentObject));
        //update position and skip past comma
        pos += newPos + 1;

    }
    free(currentObject);
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