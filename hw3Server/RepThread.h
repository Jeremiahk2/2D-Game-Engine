#ifndef REPTHREAD_H
#define REPTHREAD_H
#include <zmq.hpp>
#include <thread>
#define MESSAGE_LIMIT 1024

struct ClientStruct {
    std::thread* thread;
    RepThread* repThread;
};

struct CharStruct {
    int id;
    float x;
    float y;
};

class RepThread
{
private:

    int port;
    int id;

public:
    /**
    * Constructor
    */
    RepThread(int port, int id);

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
