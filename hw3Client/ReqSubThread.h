#ifndef REQSUB_H
#define REQSUB_H

#include <zmq.hpp>
#include <iostream>
#include <string>

#include "CThread.h"

#define MESSAGE_LIMIT 1024

class ReqSubThread
{

private:

    bool* stopped;
    GameWindow* rswindow;
    CThread* other;
    bool* rsbusy;
    std::condition_variable* rscv;
    Timeline* rstime;



public:
    /**
    * Create a new CThread an d initialize all of the fields.
    */
    ReqSubThread(bool* stopped, GameWindow* window, CThread* other, bool* busy, std::condition_variable* rscv,
                 Timeline *timeline);
    /**
    * Run the thread
    */
    void run();

    bool isBusy();
};


#endif