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
                if (!(i->getType())) {
                    //Move the platform by the current tic difference. Ideally currentTic - tic should be 1.
                    //TODO: Reexamine this later. Might just want to move it by 1 tic even if it misses it's mark. Moving average?
                    i->move(0, i->getSpeed().y * ticLength * (currentTic - tic));
                }
                else {
                    i->move(i->getSpeed().x * ticLength * (currentTic - tic), 0);
                }

                //If we intersected the bounds, switch direction
                if (!(i->getType())) {
                    //Check the bounds. Round to the nearest 2 decimal places to avoid floating point errors.
                    if ((int)i->getGlobalBounds().top < bounds.y || (int)i->getGlobalBounds().top > bounds.x ) {
                        i->setVSpeed(sf::Vector2f(0, i->getSpeed().y * -1.f));
                    }
                }
                else {
                    if (i->getGlobalBounds().left > bounds.y || i->getGlobalBounds().left < bounds.x ) {
                        i->setHSpeed(sf::Vector2f(i->getSpeed().x * -1.f, 0));
                    }
                }
            }
            tic = currentTic;
        }
    }
}