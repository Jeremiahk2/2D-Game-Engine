#include "CThread.h"
//Correct version

std::mutex *_mutex1;

std::condition_variable *_condition_variable1;

bool *stop;

Timeline *line;

GameWindow *window;

bool *upPressed;

bool* busy;

bool CThread::isBusy()
{
    std::lock_guard<std::mutex> lock(*_mutex1);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
    return *busy;
}

CThread::CThread(bool* upPressed, GameWindow* window, Timeline* timeline, bool* stopped,
    std::mutex* _mutex, std::condition_variable* _condition_variable, bool* busy)
{
    this->_mutex1 = _mutex;
    this->_condition_variable1 = _condition_variable;
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

            { // anonymous inner block to manage scop of mutex lock 
                //Take ownership of the lock and lock it
                std::unique_lock<std::mutex> cv_lock(*this->_mutex1);
                _condition_variable1->wait(cv_lock);
                *busy = false;
            }
            *upPressed = false;
            //Get gravity as a function of time
            float gravity = character->getGravity() * ticLength * (currentTic - tic);
            character->move(0.f, gravity);

            //Collision box for all collisions
            CBox collision;
            //Check collisions after gravity movement.
            if (window->checkCollisions(&collision)) {
                //If the collided object is not moving, correct the position by moving up one.
                if (!collision.isMoving()) {
                    // character->setPosition(character->getPosition().x, collision.getCBox().top);
                    character->move(0.f, -1.f * gravity); //Comment this and enable the above line for not getting stuck between moving platform and stationary.

                    //Enable jumping. TODO: Rename variable to better fit. canJump? canJump(bool)?
                    *upPressed = true;
                }
                else {
                    //If the collision IS moving, AND the character is above the platform, correct the position upwards.
                    //This helps prevent the character from getting stuck on the platform after jumping off.
                    MovingPlatform *temp = (MovingPlatform *)collision.getPlatform();
                    if (temp->getType()) {
                        //Round to 2 decimal places to avoid floating point errors.
                        if ((int)((collision.getCBox().top + gravity) * 100) >= (int)((character->getPosition().y) * 100)) {
                            character->setPosition(character->getPosition().x, temp->getGlobalBounds().top);
                            *upPressed = true;
                        }
                        character->move(temp->getLastMove());
                    }
                    else {
                        if ((int)((collision.getCBox().top + gravity + abs(temp->getLastMove().y)) * 100) >= (int)(character->getPosition().y * 100)) {
                            //If the platform is moving downward
                            if (temp->getLastMove().y > 0) {
                                //Set it's position to the top of the platform
                                character->setPosition(character->getPosition().x, collision.getCBox().top);
                            }
                            //If the platform is moving upwards
                            else {
                                //Notify main that it can update the visuals TODO: Change this to notify input that it is good to go?
                                //Set the position as one move above the next potential move from MThread. This is a reason why they need to be the same tic.
                                character->setPosition(character->getPosition().x, collision.getCBox().top + temp->getLastMove().y);
                            }
                            *upPressed = true;
                        }
                    }
                }
            }
            //Notify main of the status of upPressed
            //Need to do this until it works
            /*while (busy) {
                _condition_variable1->notify_all();
            }
            *busy = true;*/
            tic = currentTic;
            
        }
    }
}