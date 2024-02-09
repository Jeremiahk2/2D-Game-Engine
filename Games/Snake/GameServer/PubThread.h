#ifndef PUBTHREAD_H
#define PUBTHREAD_H
#include <zmq.hpp>
#include <list>
#include <iostream> //Remove later. Testing purposes only
#include "Timeline.h"
#include "MovingPlatform.h"
#include "EventManager.h"
#include "ScriptManager.h"
#include <libplatform/libplatform.h>
#define MESSAGE_LIMIT 1024

class PubThread
{
private:

    /**
    * The timneline associated with this thread
    */
    Timeline* timeline;
    /**
    * The mutex for mutual exclusion
    */
    std::mutex* mutex;

    EventManager* manager;

    int* highScore;

public:
    /**
    * Constructor
    */
    PubThread(Timeline* timeline, int *highScore, std::mutex *m, EventManager *manager);

    /**
    * run the program. It is responsible for the movement of a platform, which goes back and forth between two bounds.
    */
    void run();
};
#endif
