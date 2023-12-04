#include <zmq.hpp>
#include <string>
#include <iostream>
#include <list>

#include "MovingPlatform.h"
#include "Character.h"
#include "Platform.h"
#include "RepThread.h"
#include "PubThread.h"
#include "EventManager.h"
#include "Handlers.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <list>
#include <v8.h>

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

void run_rep(RepThread* fe) {
    fe->run();
}
void run_pub(PubThread* fe) {
    fe->run();
}

struct ClientStruct {
    std::thread* thread;
    RepThread* repThread;
};

int main() {
    //This is here to avoid rendering texture issues. Apparently OpenGL will not make a handle unless you make a window and set it active first.
    //So if you try to load textures in a certain order it fails.
    //I figured it out by reading this github post: https://github.com/Furthen64/hurkalumo/issues/24
    //I thought about putting actual OpenGL code here, but it might not work on other platforms, so this'll have to do.
    sf::RenderWindow window;
    window.setActive();

    //Create MovingPlatform and add it to the window
    MovingPlatform moving(PLAT_SPEED, 1, 150.f, 500.f);
    moving.setSize(sf::Vector2f(100.f, 15.f));
    moving.setFillColor(sf::Color(100, 250, 50));
    moving.setBounds(150, 400);

    MovingPlatform vertMoving(PLAT_SPEED, false, 0.f, 500.f);
    vertMoving.setSize(sf::Vector2f(50.f, 15.f));
    vertMoving.setFillColor(sf::Color::Magenta);
    vertMoving.setBounds(500, 200);

    //Add Moving Platforms to the list.
    std::list<MovingPlatform*> movings;
    movings.push_front(&moving);
    movings.push_front(&vertMoving);
    
    //DONE SETTING UP GAME OBJECTS

    //Set up timelines
    Timeline global;
    Timeline FrameTime(&global, TIC);

    //Set up EventManage for server
    EventManager manager(&global);
    std::string serverClosed("Server_Closed");
    std::string clientClosed("Client_Closed");
    std::list<std::string> types;
    types.push_back(clientClosed);
    types.push_back(serverClosed);
    manager.registerEvent(types, new ClosedHandler);

    //Add server closed event.
    Event e;
    e.time = GAME_LENGTH; //GAME_LENGTH into the future
    e.type = "Server_Closed";
    std::string message = "Server Closed";
    Event::variant messageVariant;
    messageVariant.m_Type = Event::variant::TYPE_STRING;
    messageVariant.m_asString = message.data();
    e.parameters.insert({ "message", messageVariant });
    manager.raise(e);

    //Set up time variables
    int64_t tic = 0;
    int64_t currentTic = 0;
    double scale = 1.0;
    double ticLength;

    //Set up thread variables
    bool stopped = false;
    std::mutex mutex;
    std::condition_variable cv;
    bool upPressed = false; //Should be named "canJump"

    //Starting server processes...
    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    repSocket.bind("tcp://localhost:5556");

    //Creating server information structures...
    std::map<int, std::shared_ptr<GameObject>> characters;
    std::list<ClientStruct> clientThreads;
    int numClients = 0;
    int numCharacters = 0;
    int availPort = 5557;

    //Create and run publisher thread
    PubThread pubthread(&FrameTime, &movings, &characters, &mutex, &manager);
    std::thread second(run_pub, &pubthread);


    //Begin main game loop
    while (true) {

        //Check to see if a new tic has gone by
        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();
        //It has, do stuff
        if (currentTic > tic) {
            //Check for new clients.
            zmq::message_t request;
            zmq::recv_result_t received(repSocket.recv(request, zmq::recv_flags::none));
            
            //Client received, set up ID and port
            int newPort = availPort++;
            int id = numClients++;

            //Create client struct and thread
            ClientStruct newClient;
            newClient.repThread = new RepThread(newPort, id, &characters, &mutex, &FrameTime, &manager);
            newClient.thread = new std::thread(run_rep, newClient.repThread);
            clientThreads.push_back(newClient);

            //Return the ID and port to the client.
            std::stringstream stream;
            stream << id << ' ' << newPort;
            std::string rtnString;
            std::getline(stream, rtnString);

            zmq::message_t reply(rtnString.size() + 1);
            memcpy(reply.data(), rtnString.data(), rtnString.size() + 1);
            repSocket.send(reply, zmq::send_flags::none);
            //Done processing new client.
        }
    }

    //Join with the threads and free their information
    for (ClientStruct i : clientThreads) {
        i.thread->join();
        delete i.repThread;
        delete i.thread;
    }
    return EXIT_SUCCESS;
}