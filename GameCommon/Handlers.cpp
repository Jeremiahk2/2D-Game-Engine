#include "Handlers.h"

void CollisionHandler::onEvent(Event e)
{
    GameObject* collision;
    Character* character;
    bool* upPressed;
    bool* doGravity;
    float ticLength;
    int differential;
    try {
        collision = e.parameters.at(std::string("collision")).m_asGameObject;
        character = (Character*)e.parameters.at(std::string("character")).m_asGameObject;
        upPressed = e.parameters.at(std::string("upPressed")).m_asBoolP;
        doGravity = e.parameters.at(std::string("doGravity")).m_asBoolP;
        ticLength = e.parameters.at(std::string("ticLength")).m_asFloat;
        differential = e.parameters.at(std::string("differential")).m_asInt;
    }
    catch (std::out_of_range) {
        std::cout << "Parameters for CollisionHandler are wrong";
        exit(3);
    }

    float oneHalfTicGrav = (character->getGravity() * ticLength) / 2;
    //If the collided object is not moving, correct the position by moving up one.
                    //NOTE: This should be impossible unless a platform was generated at the player's location (Since it's not a moving platform).
    if (collision->getObjectType() == Platform::objectType) {
        Platform* temp = (Platform*)collision;
        character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().height - oneHalfTicGrav);
        //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
        *upPressed = true;
        //We just teleported up to the top of a stationary platform, no need for gravity.
        *doGravity = false;
    }
    else if (collision->getObjectType() == MovingPlatform::objectType) {
        MovingPlatform* temp = (MovingPlatform*)collision;
        float platSpeed = (float)temp->getSpeedValue() * (float)ticLength * (float)(differential);
        float oneHalfTicPlat = ((float)temp->getSpeedValue() * (float)ticLength) / 2;

        //If the platform is moving horizontally.
        if (temp->getMovementType()) {
            //Since gravity hasn't happened yet, this must be a collision where it hit us from the x-axis, so put the character to the side of it.
            if (platSpeed < 0.f) {
                //If the platform is moving left, set us to the left of it.
                character->setPosition(temp->getGlobalBounds().left - character->getGlobalBounds().width - abs(oneHalfTicPlat), character->getPosition().y);
            }
            else {
                //If the platform is moving right, set us to the right of it
                character->setPosition(temp->getGlobalBounds().left + temp->getGlobalBounds().width + abs(oneHalfTicPlat), character->getPosition().y);
            }
        }
        //If the platform is moving vertically
        else {
            //If the platform is currently moving upwards
            if (platSpeed < 0.f) {
                //At this point, we know that we have been hit by a platform moving upwards, so correct our position upwards.
                character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().height - abs(oneHalfTicPlat));
                //We are above a platform, we can jump.
                *upPressed = true;
                //We just got placed above a platform, no need to do gravity.
                *doGravity = false;
            }
            //If the platform is moving downwards
            else {
                //Gravity will (probably) take care of it, but just in case, correct our movement to the bottom of the platform.
                character->setPosition(character->getPosition().x, temp->getGlobalBounds().top + temp->getGlobalBounds().height + oneHalfTicPlat);
            }
        }
    }
    else if (collision->getObjectType() == SideBound::objectType) {
        SideBound* sb = (SideBound*)collision;
        sb->onCollision();
    }
}

void MovementHandler::onEvent(Event e)
{
    Character *character;
    try {
        character = (Character*)e.parameters.at(std::string("character")).m_asGameObject;

        //Unless left is specified
        if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::LEFT && character->getSpeed().x != CHAR_SPEED) {
            character->setSpeed(sf::Vector2f(-CHAR_SPEED, 0));
        }
        else if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::RIGHT && character->getSpeed().x != -CHAR_SPEED) {
            character->setSpeed(sf::Vector2f(CHAR_SPEED, 0));
        }
        else if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::UP && character->getSpeed().y != CHAR_SPEED) {
            character->setSpeed(sf::Vector2f(0, -CHAR_SPEED));
        }
        else if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::DOWN && character->getSpeed().y != -CHAR_SPEED) {
            character->setSpeed(sf::Vector2f(0, CHAR_SPEED));
        }
    } 
    catch (std::out_of_range) {
        std::cout << "Parameters for MovementHandler messed up";
        exit(3);
    }
}

GravityHandler::GravityHandler(EventManager *em, GameWindow *window, ScriptManager *sm)
{
    this->em = em;
    this->window = window;
    this->sm = sm;
}

void GravityHandler::onEvent(Event e)
{
    //Get event parameters
    Character* character;
    try {
        character = (Character *)e.parameters.at(std::string("character")).m_asGameWindow;
    }
    catch (std::out_of_range) {
        std::cout << "Parameters incorrect in GravityHandler";
        exit(3);
    }
    //Only do ANY of this if we are moving.
    if (!(character->getSpeed().x == 0 && character->getSpeed().y == 0)) {
        GameObject* collision;
        sf::Vector2f oldBack = character->getPosition();
        if (character->trail.size() != 0) {
            //Take a reference to the back.
            Character* back = character->trail.back();
            //Save it for if we found an apple.
            oldBack = back->getPosition();
            //Pop it off and add it to the front, at the same position character is at.
            character->trail.pop_back();
            back->setPosition(character->getPosition());
            character->trail.push_front(back);
        }
        //Move character forward
        sm->addArgs(character);
        sm->runOne("move_character");
        //character->move(character->getSpeed());
        //Erase the position my character is in from the unoccupied list.
        for (std::list<sf::Vector2f>::iterator it = character->unoccupied.begin(); it != character->unoccupied.end();)
        {
            //Erase the position my character is in from the unoccupied list.
            if (*it == character->getPosition()) {
                character->unoccupied.erase(it);
                break;
            }
            else {
                it++;
            }
        }

        //Check collisions after character movement.
        if (window->checkCollisions(&collision)) {
            //If the collision was a wall or its tail.
            if ((collision->getObjectType() == Character::objectType || collision->getObjectType() == Platform::objectType) && !character->isDead()) {
                Event death;
                character->died();
                Event::variant characterVariant;
                characterVariant.m_Type = Event::variant::TYPE_GAMEOBJECT;
                characterVariant.m_asGameObject = character;
                death.parameters.insert({ "character", characterVariant });
                death.type = "death";
                death.time = e.time;
                death.order = e.order + 1;
                em->raise(death);
            }
            //If the collision was an apple (Moving platform)
            else if (collision->getObjectType() == MovingPlatform::objectType && !character->isDead()) {
                //Hit apple
                Character* newCharacter = new Character;
                //Set the new character piece at where the old back used to be (Should be blank now).
                newCharacter->setPosition(oldBack);
                character->trail.push_back(newCharacter);
                window->addGameObject(newCharacter);

                //Generate new apple position.
                srand(time(NULL));
                int appleIndex = rand() % character->unoccupied.size();
                int count = 0;
                sf::Vector2f newPosition;
                std::list<sf::Vector2f>::iterator it = character->unoccupied.begin();
                bool found = false;
                for (sf::Vector2f i : character->unoccupied) {
                    if (count == appleIndex) {
                        newPosition = i;
                        found = true;
                        break;
                    }
                    it++;
                    count++;
                }
                //Change the apple's position to the generated one.
                character->apple->setPosition(newPosition);
                character->length++;
            }
            else {
                character->unoccupied.push_back(oldBack);
            }
        }
        else {
            character->unoccupied.push_back(oldBack);
        }
    }
}

SpawnHandler::SpawnHandler(GameWindow* window)
{
    this->window = window;
}

void SpawnHandler::onEvent(Event e)
{
    Character* character;
    try {
        character = (Character*)e.parameters.at("character").m_asGameObject;
    }
    catch (std::out_of_range) {
        std::cout << "Invalid arguments SpawnHandler" << std::endl;
        exit(3);
    }
    //Clear trail
    character->length = 0;
    character->trail.clear();
    character->unoccupied.clear();
    //Repopulate unoccupied list
    for (int i = 0; i < 780 / character->getSize().x; i++) {
        for (int j = 0; j < 580 / character->getSize().y; j++) {
            //Skip first square (Character will be here).
            if (!(i == 0 && j== 0)) {
                character->unoccupied.push_back(sf::Vector2f(i * character->getSize().x + 10, j * character->getSize().y + 10));
            }
        }
    }
    //Clear trail from window.
    window->clearStaticObjects();

    //Regenerate apple
    int appleIndex = rand() % character->unoccupied.size();
    int count = 0;
    sf::Vector2f newPosition;
    std::list<sf::Vector2f>::iterator it = character->unoccupied.begin();
    for (sf::Vector2f i : character->unoccupied) {
        if (count == appleIndex) {
            newPosition = i;
            break;
            //character->unoccupied.erase(it);
        }
        it++;
        count++;
    }
    //Change the apple's position to the generated one.
    character->apple->setPosition(newPosition);
    window->addGameObject(character->apple);
    //Add the boundaries back to the window
    Platform *bottom = new Platform;
    bottom->setSize(sf::Vector2f(800, 10.f));
    bottom->setFillColor(sf::Color(100, 0, 0));
    bottom->setPosition(sf::Vector2f(0, 590));
    window->addGameObject(bottom);

    Platform *right = new Platform;
    right->setSize(sf::Vector2f(10, 580));
    right->setFillColor(sf::Color(100, 0, 0));
    right->setPosition(sf::Vector2f(790, 10));
    window->addGameObject(right);

    Platform *left = new Platform;
    left->setSize(sf::Vector2f(10, 580));
    left->setFillColor(sf::Color(100, 0, 0));
    left->setPosition(sf::Vector2f(0, 10));
    window->addGameObject(left);

    Platform* top = new Platform;
    top->setSize(sf::Vector2f(800, 10.f));
    top->setFillColor(sf::Color(100, 0, 0));
    top->setPosition(sf::Vector2f(0, 0));
    window->addGameObject(top);
    //Respawn the character
    character->respawn();
}

DeathHandler::DeathHandler(EventManager* em, ScriptManager*sm)
{
    this->em = em;
    this->sm = sm;
}

void DeathHandler::onEvent(Event e)
{
    Event::events.insert_or_assign( e.guid, &e );
    sm->addArgs(&e);
    sm->runOne("handle_death", false);
}

ClosedHandler::ClosedHandler()
{
    em = NULL;
}

ClosedHandler::ClosedHandler(EventManager* em)
{
    this->em = em;
}

void ClosedHandler::onEvent(Event e)
{
    if (e.type == "Server_Closed") {
        //we are on the server, need to forcefully exit.
        try {
            std::cout << e.parameters.at("message").m_asString << std::endl;
            exit(1);
        }
        catch (std::out_of_range) {
            std::cout << "Invalid arguments ClosedHandler" << std::endl;
            exit(3);
        }
    }
    else if (e.type == "Client_Closed") {
        try {
            //Only execute if we are on the client (If there is no window, we are on the server)
            if (em && em->getWindow()) {
                std::cout << e.parameters.at("message").m_asString << std::endl;
                exit(1);
            }
        }
        catch (std::out_of_range) {
            std::cout << "Invalid arguments ClosedHandler (Client)" << std::endl;
            exit(3);
        }
    }
}

StopHandler::StopHandler()
{
}

void StopHandler::onEvent(Event e)
{
    Character* character;
    try {
        character = (Character*)e.parameters.at(std::string("character")).m_asGameObject;
    }
    catch (std::out_of_range) {
        std::cout << "Parameters for CollisionHandler are wrong";
        exit(3);
    }
    //Stop the character's movement.
    character->setSpeed(0);
    character->setSpeed(sf::Vector2f(0, 0));

}
