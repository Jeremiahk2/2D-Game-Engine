#include "Handlers.h"

void CollisionHandler::onEvent(Event e)
{
    GameObject* collision = e.parameters.at(std::string("collision")).m_asGameObject;
    Character* character = (Character *)e.parameters.at(std::string("character")).m_asGameObject;
    bool* upPressed = e.parameters.at(std::string("upPressed")).m_asBoolP;
    bool *doGravity = e.parameters.at(std::string("doGravity")).m_asBoolP;
    float ticLength = e.parameters.at(std::string("ticLength")).m_asFloat;
    int differential = e.parameters.at(std::string("differential")).m_asInt;
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
        float oneHalfTicPlat = (float)temp->getSpeedValue() * (float)ticLength;

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
    else if (collision->getObjectType() == DeathZone::objectType) {
        character->respawn();
    }
    else if (collision->getObjectType() == SideBound::objectType) {
        SideBound* sb = (SideBound*)collision;
        sb->onCollision();
    }
}

void MovementHandler::onEvent(Event e)
{
}

void GravityHandler::onEvent(Event e)
{
}

void DisconnectHandler::onEvent(Event e)
{
}
