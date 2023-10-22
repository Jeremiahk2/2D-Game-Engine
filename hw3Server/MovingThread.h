#ifndef MOVINGTHREAD_H
#define MOVINGTHREAD_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "MovingPlatform.h"
#include "Timeline.h"
using namespace std;

class MovingThread
{
    private:
    /**
    * Is the thread busy
    * TODO: Might not be needed, check back later
    */
    bool busy;

    /**
    * The index of the thread
    * TODO: Also might not be needed, check back later.
    */
    int i;

    /**
    * The other thread
    * TODO: May not be needed, check back later.
    */
    MovingThread *other;

    /**
    * The mutex to handle mutual exclusion. Share this with CThread
    */
    std::mutex *_mutex;

    /**
    * The condition variable to handle notifications
    */
    std::condition_variable *_condition_variable;

    /**
    * The list of moving platforms in the window.
    */
    list<MovingPlatform*> *movingPlatforms;

    /**
    * Boolean for whether or not to stop the thread or not.
    * Warning: May need to set stop to true and notify the thread as well.
    */
    bool *stopped;

    /**
    * The timeline for MThread. Should have same tic rate as CThread
    */
    Timeline *timeline;


    public :
        /**
        * Constructor
        */
        MovingThread(Timeline *timeline, bool *stopped, int i, MovingThread *other, std::mutex *_mutex, std::condition_variable *_condition_variable, list<MovingPlatform*> *movingPlatforms);

        /**
        * Not used. Possibly not needed.
        */
        bool isBusy();

        /**
        * run the program. It is responsible for the movement of a platform, which goes back and forth between two bounds.
        */
        void run();
};
#endif