#include <zmq.hpp>
#include <string>
#include <iostream>
#include <list>

#include "MovingPlatform.h"
#include "Character.h"
#include "Platform.h"
#include "CBox.h"
#include "GameWindow.h"
#include "MovingThread.h"
#include "RepThread.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>

#define MESSAGE_LIMIT 1024

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

#define CHAR_SPEED 100.f

// #define GRAV_SPEED 160.f
#define GRAV_SPEED 160

//Speed is calculated in pixels per second.
#define PLAT_SPEED 100.f

#define JUMP_SPEED 420.f

#define JUMP_TIME .5

#define TIC 8 //Change this to try out different tic rates

#define MESSAGE_LIMIT 1024 //Limit on string length for network messages

/**
 * Wrapper function because threads can't take pointers to member functions.
 */
void run_moving(MovingThread* fe)
{
    fe->run();
}

void run_rep(RepThread* fe) {
    fe->run();
}
//Struct for other client's characters
struct CharStruct {
    int id;
    float x;
    float y;
};

int main() {

    //Create MovingPlatform and add it to the window
    MovingPlatform moving(PLAT_SPEED, 1, 150.f, 500.f);
    moving.setSize(sf::Vector2f(100.f, 15.f));
    moving.setFillColor(sf::Color(100, 250, 50));
    moving.setBounds(150, 400);

    MovingPlatform vertMoving(PLAT_SPEED, false, 600.f, 500.f);
    vertMoving.setSize(sf::Vector2f(50.f, 15.f));
    vertMoving.setFillColor(sf::Color::Magenta);
    vertMoving.setBounds(500, 200);

    //Add Moving Platforms to the list.
    list<MovingPlatform*> movings;
    movings.push_front(&moving);
    movings.push_front(&vertMoving);

    //Boolean so that the threads know that main has stopped
    bool stopped = false;

    std::mutex m;
    std::condition_variable cv;
    //Should be named "canJump"
    bool upPressed = false;

    //This should allow altering all timelines through global (pausing).
    Timeline global;
    //most systems should really have the same tic rate as of now. 
    Timeline MPTime(&global, TIC);
    Timeline FrameTime(&global, TIC);
    MovingThread mthread(&MPTime, &stopped, 0, NULL, &m, &cv, &movings);

    std::thread first(run_moving, &mthread);

    //Maintain time information
    int64_t tic = 0;
    int64_t currentTic = 0;
    double scale = 1.0;
    double ticLength;

    //Starting server processes...
    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    zmq::socket_t pubSocket(context, zmq::socket_type::pub);
    repSocket.bind("tcp://localhost:5556");
    pubSocket.bind("tcp://localhost:5555");

    //Creating server information structures...
    CharStruct characters[10];
    int numClients = 0;
    int numCharacters = 0;
    int availPort = 5557;

    std::list<std::thread> clientThreads;

    //Begin main game loop
    while (true) {

        //Check to see if a new tic has gone by
        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();
        //It has, do stuff
        if (currentTic > tic) {
            //Check for new clients.
            zmq::message_t update;
            zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));
            
            //Client received, create thread for client
            int newPort = availPort++;
            int id = numClients++;
            RepThread newRep(availPort, numClients);
            //clientThreads.push_front(new std::thread(run_rep, &newRep)); Use new keyword?
            std::thread newThread(run_rep, &newRep);
            //Malloc room for thread and then store pointer to it in clientThreads?

        }
    }
    return EXIT_SUCCESS;
}