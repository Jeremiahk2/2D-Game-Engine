#ifndef PUBTHREAD_H
#define PUBTHREAD_H
#include <zmq.hpp>

struct CharStruct {
    int id;
    float x;
    float y;
};

class PubThread
{
private:



public:
    /**
    * Constructor
    */
    PubThread();

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
