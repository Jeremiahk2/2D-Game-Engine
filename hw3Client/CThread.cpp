#include "CThread.h"
//Correct version

bool CThread::isBusy()
{
    std::lock_guard<std::mutex> lock(*mutex);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
    return *busy;
}

CThread::CThread(bool* upPressed, GameWindow* window, Timeline* timeline, bool* stopped,
    std::mutex* m, std::condition_variable* cv, bool* busy)
{
    this->mutex = m;
    this->cv = cv;
    this->stop = stopped;
    this->line = timeline;
    this->window = window;
    this->upPressed = upPressed;
    this->busy = busy;
}

void CThread::run() {
    int64_t tic = 0;
    int64_t currentTic;
    float ticLength;

    Character *character = (Character *)window->getPlayableObject();
    while (!(*stop)) {
        currentTic = line->getTime();
        if (currentTic > tic) {
            ticLength = line->getRealTicLength();

            *upPressed = false;
            //Get gravity as a function of time
            float gravity = character->getGravity() * ticLength * (currentTic - tic);
            {
                //Character is entering an inconsistent state, lock it.
                std::lock_guard<std::mutex> lock(*mutex);
                GameObject* collision = nullptr;
                bool doGravity = true;
                if (window->checkCollisions(collision)) {
                    //If the collided object is not moving, correct the position by moving up one.
                    //NOTE: This should be impossible unless a platform was generated at the player's location (Since it's not a moving platform).
                    if (collision->getObjectType() == Platform::objectType) {
                        Platform* temp = (Platform*)collision;
                        character->setPosition(character->getPosition().x, temp->getGlobalBounds().top + character->getGlobalBounds().height + 1.f);
                        //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
                        *upPressed = true;
                        //We just teleported up to the top of a stationary platform, no need for gravity.
                        doGravity = false;
                    }
                    else if (collision->getObjectType() == MovingPlatform::objectType){
                        MovingPlatform* temp = (MovingPlatform*)collision;
                        float platSpeed = (float)temp->getSpeedValue() * (float)ticLength * (float)(currentTic - tic);

                        //If the platform is moving horizontally.
                        if (temp->getMovementType()) {
                            //Since gravity hasn't happened yet, this must be a collision where it hit us from the x-axis, so put the character to the side of it.
                            if (platSpeed < 0.f) {
                                //If the platform is moving left, set us to the left of it.
                                character->setPosition(temp->getGlobalBounds().left - character->getGlobalBounds().left - 1.f, character->getPosition().y);
                            }
                            else {
                                //If the platform is moving right, set us to the right of it
                                character->setPosition(temp->getGlobalBounds().left + temp->getGlobalBounds().width + 1.f, character->getPosition().y);
                            }
                        }
                        //If the platform is moving vertically
                        else {
                            //If the platform is currently moving upwards
                            if (platSpeed < 0.f) {
                                //At this point, we know that we have been hit by a platform moving upwards, so correct our position upwards.
                                character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().top - 1.f);
                                //We are above a platform, we can jump.
                                *upPressed = true;
                                //We just got placed above a platform, no need to do gravity.
                                doGravity = false;
                            }
                            //If the platform is moving downwards
                            else {
                                //Gravity will (probably) take care of it, but just in case, correct our movement to the bottom of the platform.
                                character->setPosition(character->getPosition().x, temp->getGlobalBounds().top + temp->getGlobalBounds().height + 1.f);
                            }
                        }
                    }
                }
                //At this point, character shouldn't be colliding with anything at all.
                if (doGravity) {
                    character->move(0.f, gravity);
                    //Check collisions after gravity movement.
                    if (window->checkCollisions(collision)) {
                        //If the collided object is a stationary platform, correct the position to be on top of the platform.
                        if (collision->getObjectType() == Platform::objectType) {
                            Platform* temp = (Platform*)collision;
                            character->setPosition(character->getPosition().x, temp->getGlobalBounds().top + character->getGlobalBounds().height + 1.f);
                            //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
                            *upPressed = true;
                        }
                        else if (collision->getObjectType() == MovingPlatform::objectType) {
                            MovingPlatform* temp = (MovingPlatform*)collision;
                            float platSpeed = (float)temp->getSpeedValue() * (float)ticLength * (float)(currentTic - tic);

                            //If the platform is moving horizontally.
                            if (temp->getMovementType()) {
                                //Gravity has happened, which means we moved down into a platform that was already there. Move along with it.
                                character->move(platSpeed, 0);
                            }
                            //If the platform is moving vertically
                            else {
                                //If the platform is currently moving upwards
                                if (platSpeed < 0.f) {
                                    //At this point, we know that we have been hit by a platform moving upwards, so correct our position upwards.
                                    character->setPosition(character->getPosition().x, temp->getGlobalBounds().top - character->getGlobalBounds().top - 1.f);
                                    //We are above a platform, we can jump.
                                    *upPressed = true;
                                    //We just got placed above a platform, no need to do gravity.
                                    doGravity = false;
                                }
                                //If the platform is moving downwards
                                else {
                                    //Gravity will (probably) take care of it, but just in case, correct our movement to the bottom of the platform.
                                    character->setPosition(character->getPosition().x, temp->getGlobalBounds().top + temp->getGlobalBounds().height + 1.f);
                                }
                            }
                        }
                    }
                }
            }
            tic = currentTic;
            
        }
    }
}