#ifndef REPTHREAD_H
#define REPTHREAD_H
#include <zmq.hpp>
#include <thread>
#include <map>
#include <mutex>
#include <iostream> //TODO: Remove
#include "Timeline.h"
#include "Character.h"
#include "EventManager.h"
#define GAME_LENGTH 10000000000
#define MESSAGE_LIMIT 1024

class RepThread
{
private:
    /**
    * The port that this thread is/will be binded to.
    */
    int port;
    /**
    * The ID of this thread and the client connecting to it.
    */
    int id;
    /**
    * mutex for mutual exclusion
    */
    std::mutex* mutex;
    /**
    * the timeline associated with this thread
    */
    Timeline* time;

    EventManager* manager;

    int* highScore;
public:
    /**
    * Constructor
    */
    RepThread(int port, int id, int *highScore, std::mutex *m, Timeline *time, EventManager *manager);

    /**
    * run the program. It is responsible for the movement of a platform, which goes back and forth between two bounds.
    */
    void run();
};
#endif
