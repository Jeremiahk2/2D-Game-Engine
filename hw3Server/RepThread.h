#ifndef REPTHREAD_H
#define REPTHREAD_H
#include <zmq.hpp>
#include <thread>
#include <map>
#include <mutex>
#include "CommonTypes.h"
#define MESSAGE_LIMIT 1024

class RepThread
{
private:

    int port;
    int id;
    std::map<int, CharStruct> *characters;
    std::mutex* mutex;

public:
    /**
    * Constructor
    */
    RepThread(int port, int id, std::map<int, CharStruct>* characters, std::mutex *m);

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