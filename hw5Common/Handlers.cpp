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
    GameWindow* window;
    float ticLength;
    int differential;
    try {
        window = e.parameters.at(std::string("window")).m_asGameWindow;
        ticLength = e.parameters.at(std::string("ticLength")).m_asFloat;
        differential = e.parameters.at(std::string("differential")).m_asInt;
    }
    catch (std::out_of_range) {
        std::cout << "Parameters for MovementHandler messed up";
        exit(3);
    }
    Character* character = (Character*)window->getPlayableObject();
    float charSpeed = (float)character->getSpeed().x * (float)ticLength * (float)(differential);
    sf::Vector2f speedVector;
    sf::Vector2f oppositeVector;
    //Unless left is specified
    if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::LEFT) {
        speedVector.x = charSpeed * -1;
    }
    else if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::RIGHT) {
        speedVector.x = charSpeed;
    }
    else if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::UP) {
        speedVector.y = character->getJumpSpeed() * (float)ticLength * (float)differential;
    }

    character->move(speedVector);

    GameObject* collision;
    //Check for collisions
    if (window->checkCollisions(&collision)) {
        //If the collided platform is not moving, just correct the position of Character back.
        if (collision->getObjectType() == Platform::objectType) {
            character->move(-1.f * speedVector);
        }
        //if the collided platform is moving, move the character back AND move them along with the platform.
        else if (collision->getObjectType() == MovingPlatform::objectType) {
            MovingPlatform* temp = (MovingPlatform*)collision;
            //Convert plat's speed to pixels per tic
            float oneHalfTicSpeed = (character->getSpeed().x * ticLength) / 2;
            if (e.parameters.at(std::string("direction")).m_asInt == MovementHandler::LEFT) {
                character->setPosition(temp->getGlobalBounds().left + temp->getGlobalBounds().width + oneHalfTicSpeed, character->getPosition().y);
            }
            else {
                character->setPosition(temp->getGlobalBounds().left - character->getGlobalBounds().width - oneHalfTicSpeed, character->getPosition().y);
            }
        }
        else if (collision->getObjectType() == SideBound::objectType) {
            SideBound* temp = (SideBound*)collision;
            temp->onCollision();
        }
    }
}

GravityHandler::GravityHandler(EventManager *em)
{
    this->em = em;
}

void GravityHandler::onEvent(Event e)
{
    //Get event parameters
    GameWindow* window;
    bool* upPressed;
    float ticLength;
    int differential;
    float nonScalableTicLength;
    bool* doGravity;
    try {
        window = e.parameters.at(std::string("window")).m_asGameWindow;
        upPressed = e.parameters.at(std::string("upPressed")).m_asBoolP;
        ticLength = e.parameters.at(std::string("ticLength")).m_asFloat;
        differential = e.parameters.at(std::string("differential")).m_asInt;
        nonScalableTicLength = e.parameters.at(std::string("nonScalableTicLength")).m_asFloat;
        doGravity = e.parameters.at(std::string("doGravity")).m_asBoolP;
    }
    catch (std::out_of_range) {
        std::cout << "Parameters incorrect in GravityHandler";
        exit(3);
    }

    //Set up our own variables.
    Character* character = (Character*)window->getPlayableObject();
    float oneHalfTicGrav = (character->getGravity() * ticLength) / 2;
    float gravity = character->getGravity() * ticLength * (differential);

    GameObject* collision;
    if (doGravity) {
        character->move(0.f, gravity);
        //Check collisions after gravity movement.
        if (window->checkCollisions(&collision)) {

            //If the collided object is a stationary platform, correct the position to be on top of the platform.
            if (collision->getObjectType() == Platform::objectType) {
                Platform* temp = (Platform*)collision;
                character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().height - oneHalfTicGrav);
                //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
                *upPressed = true;
            }
            else if (collision->getObjectType() == MovingPlatform::objectType) {
                MovingPlatform* temp = (MovingPlatform*)collision;
                float platSpeed = (float)temp->getSpeedValue() * (float)nonScalableTicLength * (float)(differential);

                //If the platform is moving horizontally.
                if (temp->getMovementType()) {
                    //Gravity has happened, which means we moved down into a platform that was already there. Move along with it AND correct pos to the top of it.
                    character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().height - oneHalfTicGrav);
                    character->move(platSpeed, 0);
                    *upPressed = true;
                }
                //If the platform is moving vertically
                else {
                    //If the platform is currently moving upwards
                    if (platSpeed < 0.f) {
                        //At this point, we know that we have been hit by a platform moving upwards, so correct our position upwards.
                        character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().height - oneHalfTicGrav);
                        //We are above a platform, we can jump.
                        *upPressed = true;
                    }
                    //If the platform is moving downwards
                    else {
                        //Just moved down into a downward moving platform. Correct us to be on top of it.
                        character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().height - oneHalfTicGrav);
                        *upPressed = true;
                    }
                }
            }
            else if (collision->getObjectType() == DeathZone::objectType && !character->isDead()) {
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
        }
    }
}

void DisconnectHandler::onEvent(Event e)
{
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

    character->respawn();
}

DeathHandler::DeathHandler(EventManager* em)
{
    this->em = em;
}

void DeathHandler::onEvent(Event e)
{
    Event spawn;
    spawn = e;
    spawn.type = "spawn";
    spawn.time = e.time + 3000; //Respawn three seconds later.
    spawn.order = e.order + 1;
    em->raise(spawn);
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

//NetworkHandler::NetworkHandler(EventManager* em)
//{
//    this->em = em;
//}
//
//void NetworkHandler::onEvent(Event e)
//{
//    zmq::context_t context(2);
//    zmq::socket_t socket(context, zmq::socket_type::rep);
//    socket.connect("tcp://localhost:5556");
//    //try {
//    //    socket = e.parameters.at("socket").m_asSocket;
//    //}
//    //catch (std::out_of_range) {
//    //    std::cout << "Invalid socket parameter in NetworkHandler" << std::endl;
//    //    exit(3);
//    //}
//    if (e.type == "Client Closed") {
//        e.time = GAME_LENGTH - em->getTimeline()->getGlobalTime();
//        std::string rtnString = e.toString();
//        zmq::message_t reply(rtnString.size() + 1);
//        memcpy(reply.data(), rtnString.data(), rtnString.size() + 1);
//        socket.send(reply, zmq::send_flags::none);
//        zmq::message_t update;
//        zmq::recv_result_t received(socket.recv(update, zmq::recv_flags::none));
//    }
//}
