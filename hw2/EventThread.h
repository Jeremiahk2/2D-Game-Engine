#ifndef ETHREAD_H
#define ETHREAD_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <list>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "MovingPlatform.h"
#include "Timeline.h"
#include "GameWindow.h"
using namespace std;

class EventThread
{

    GameWindow ewindow;

    Timeline* eglobal;

    bool* estopped;

    std::condition_variable* ecv;


public:
    /**
    * Create a new EThread and initialize all of the fields.
    */
    EventThread(Timeline* global, bool* stopped, std::condition_variable* ecv);

    GameWindow* getWindowPointer();
    /**
    * Run the thread
    */
    void run();
};
#endif