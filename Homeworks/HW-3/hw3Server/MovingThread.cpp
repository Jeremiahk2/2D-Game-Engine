#include "MovingThread.h"

MovingThread::MovingThread(Timeline *timeline, bool *stopped, int i, MovingThread *other, std::mutex *mutex, std::condition_variable *cv, list<MovingPlatform*> *movingPlatforms) {
    this->stopped = stopped;
    this->i = i; // set the id of this thread
    if(i==0) { busy = true; }
    else { this->other = other; }
    this->mutex = mutex;
    this->cv = cv;
    this->movingPlatforms = movingPlatforms;
    this->timeline = timeline;
}

bool MovingThread::isBusy() {
    std::lock_guard<std::mutex> lock(*mutex);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
    return busy;
}

void MovingThread::run() {
    int64_t tic = 0;
    int64_t currentTic;
    float ticLength;
    while (!(*stopped)) {
        //Check to see if a new tic has gone by.
        ticLength = timeline->getRealTicLength();
        currentTic = timeline->getTime();
        if (currentTic > tic) { //It has, move platforms
            for (MovingPlatform *i : *movingPlatforms) {

                sf::Vector2i bounds = i->getBounds();
                float platSpeed = (float)i->getSpeedValue() * (float)ticLength * (float)(currentTic - tic);
                //boundx is left or bottom bound
                //If the movement is horizontal
                if ((i->getMovementType())) {
                    //If we are moving to the right
                    if (platSpeed > 0) {
                        //Then we only care about our right bound.
                        if ((int)i->getGlobalBounds().left > bounds.y) {
                            i->setSpeed(sf::Vector2f(i->getSpeedValue() * -1.f, 0));
                        }
                    }
                    //Else if we are moving left, check the left bound.
                    else if ((int)i->getGlobalBounds().left < bounds.x) {
                        i->setSpeed(sf::Vector2f(i->getSpeedValue() * -1.f, 0));
                    }
                }
                //If movement is vertical
                else {
                    //If we are going up
                    if (platSpeed < 0) {
                        //Then we only care about or top bound.
                        if ((int)i->getGlobalBounds().top < bounds.y) {
                            i->setSpeed(sf::Vector2f(0, i->getSpeedValue() * -1.f));
                        }
                    }
                    //else if we are going down, we only care about our bottom bound.
                    else if ((int)i->getGlobalBounds().top > bounds.x) {
                        i->setSpeed(sf::Vector2f(0, i->getSpeedValue() * -1.f));
                    }
                }
                //TODO: I swapped the order. Check back later

                //If the platform is moving horizontally
                if ((i->getMovementType())) {
                    i->move(platSpeed, 0);
                }
                else {
                    i->move(0, platSpeed);
                }
            }
            tic = currentTic;
        }
    }
}