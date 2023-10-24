#ifndef REQTHREAD_H
#define REQTHREAD_H

#include <zmq.hpp>
#include <iostream>
#include <string>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

#include "CThread.h"

#define MESSAGE_LIMIT 1024

class ReqThread
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
    ReqThread(bool* stopped, GameWindow* window, CThread* other, bool* busy, std::condition_variable* rscv,
                 Timeline *timeline);
    /**
    * Run the thread
    */
    void run();

    bool isBusy();
};


#endif