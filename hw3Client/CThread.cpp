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

    Character *character = window->getCharacter();
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
                CBox collision;
                if (window->checkCollisions(&collision)) {
                    //If the collided object is not moving, correct the position by moving up one.
                    if (!collision.isMoving()) {
                        character->setPosition(character->getPosition().x, collision.getCBox().top);
                        /*character->move(0.f, -1.f * gravity);*/ //Comment this and enable the above line for not getting stuck between moving platform and stationary.

                        //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
                        *upPressed = true;
                    }
                    else {
                        //If the collision IS moving, AND the character is above the platform, correct the position upwards.
                        //This helps prevent the character from getting stuck on the platform after jumping off.
                        MovingPlatform* temp = (MovingPlatform*)collision.getPlatform();
                        if (temp->getType()) {
                            //Round to 2 decimal places to avoid floating point errors.
                            if ((int)((collision.getCBox().top + abs(temp->getLastMove().y))  * 100) >= (int)((character->getPosition().y) * 100)) {
                                character->setPosition(character->getPosition().x, collision.getCBox().top);
                                *upPressed = true;
                            }
                            character->move(temp->getLastMove());
                        }
                        else {
                            if ((int)((collision.getCBox().top + abs(temp->getLastMove().y)) * 100) >= (int)(character->getPosition().y * 100)) {
                                character->setPosition(character->getPosition().x, collision.getCBox().top /*+ temp->getLastMove().y*/);
                                *upPressed = true;
                            }
                        }
                    }
                }
                else {
                    character->move(0.f, gravity);

                    //Collision box for all collisions
                    //Check collisions after gravity movement.
                    if (window->checkCollisions(&collision)) {
                        //If the collided object is not moving, correct the position by moving up one.
                        if (!collision.isMoving()) {
                            character->setPosition(character->getPosition().x, collision.getCBox().top - 1.f);
                            /*character->move(0.f, -1.f * gravity);*/ //Comment this and enable the above line for not getting stuck between moving platform and stationary.

                            //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
                            *upPressed = true;
                        }
                        else {
                            MovingPlatform* temp = (MovingPlatform*)collision.getPlatform();
                            //If the platform is moving horizontally
                            if (temp->getType()) {
                                //Round to 2 decimal places to avoid floating point errors.
                                if ((int)((collision.getCBox().top + gravity) * 100) >= (int)((character->getPosition().y) * 100)) {
                                    character->setPosition(character->getPosition().x, collision.getCBox().top - 1.f);
                                    *upPressed = true;
                                }
                                character->move(temp->getLastMove());
                            }
                            //If the platform is moving vertically
                            else {
                                if ((int)((collision.getCBox().top + gravity) * 100) >= (int)(character->getPosition().y * 100)) {
                                    //If the platform is moving downward
                                    if (temp->getLastMove().y >= 0) {
                                        //Set it's position to the top of the platform
                                        character->setPosition(character->getPosition().x, collision.getCBox().top - 1.f);
                                    }
                                    //If the platform is moving upwards
                                    else {
                                        //Set the position as one move above the next potential move from MThread. This is a reason why they need to be the same tic.
                                        character->setPosition(character->getPosition().x, collision.getCBox().top - 1.f/*+ temp->getLastMove().y*/);
                                    }
                                    *upPressed = true;
                                }
                            }
                        }
                    }
                }
            }
            cv->notify_one();
            //Character is no longer in an inconsistent state, notify other threads
            tic = currentTic;
            
        }
    }
}