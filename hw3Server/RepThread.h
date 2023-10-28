#ifndef REPTHREAD_H
#define REPTHREAD_H
#include <zmq.hpp>
#include <thread>
#include <map>
#include <mutex>
#include <iostream> //TODO: Remove
#include "Timeline.h"
#include "Character.h"
#define MESSAGE_LIMIT 1024

class RepThread
{
private:

    int port;
    int id;
    std::map<int, std::shared_ptr<GameObject>> *characters;
    std::mutex* mutex;
    Timeline* time;

public:
    /**
    * Constructor
    */
    RepThread(int port, int id, std::map<int, std::shared_ptr<GameObject>>* characters, std::mutex *m, Timeline *time);

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
