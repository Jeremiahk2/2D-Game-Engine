#ifndef CTHREAD_H
#define CTHREAD_H
//Correct version
#include <thread>
#include <condition_variable>
#include <zmq.hpp>

#include "MovingPlatform.h"
#include "Timeline.h"
#include "GameWindow.h"
#define MESSAGE_LIMIT 1024


class CThread
{

    /**
    * Mutex for handling mutual exclusion. Have this be shared among CThread and MThread at the very least.
    */
    std::mutex *mutex;

    /**
    * The condition variable for handling waits and notifications.
    */
    std::condition_variable *cv;

    /**
    * Set *stop to true when you want this thread to stop. 
    * Warning: setting this to stop when it is waiting may cause issues.
    * Make sure to set stop to true and then notify_all()
    */
    bool *stop;

    /**
    * The timeline for this thread. This should have the same ticrate as MThread
    */
    Timeline *line;

    /**
    * The window that the character and it's colliding objects are in.
    */
    GameWindow *window;

    /**
    * This is a boolean that main will need to use to know if it's okay for
    * the character to jump or not.
    */
    bool *upPressed;

    bool* busy;




    public :
        /**
        * Create a new CThread an d initialize all of the fields.
        */
        CThread(bool* upPressed, GameWindow* window, Timeline* timeline, bool* stopped,
            std::mutex* m, std::condition_variable* cv, bool *busy);
        /**
        * Run the thread
        */
        void run();

        bool isBusy();
};
#endif