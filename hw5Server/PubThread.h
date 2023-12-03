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
    * The list of moving platforms to move and publish
    */
    std::list<MovingPlatform*>* movings;
    /**
    * The list of characters to publish
    */
    std::map<int, std::shared_ptr<GameObject>>* characters;
    /**
    * The mutex for mutual exclusion
    */
    std::mutex* mutex;

    EventManager* manager;


public:
    /**
    * Constructor
    */
    PubThread(Timeline* timeline, std::list<MovingPlatform*>* movings, std::map<int, std::shared_ptr<GameObject>>* characters, std::mutex *m, EventManager *manager);

    /**
    * run the program. It is responsible for the movement of a platform, which goes back and forth between two bounds.
    */
    void run();
};
#endif
