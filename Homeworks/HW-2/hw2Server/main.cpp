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
    repSocket.bind("tcp://localhost:5555");
    pubSocket.bind("tcp://localhost:5556");

    //Creating server information structures...
    CharStruct characters[10];
    int numClients = 0;
    int numCharacters = 0;

    //Begin main game loop
    while (true) {

        //Check to see if a new tic has gone by
        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();
        //It has, do stuff
        if (currentTic > tic) {

            //We are expecting numClients to connect, so we loop for them
            for (int i = 0; i < numClients; i++) {

                //Receive message from client
                zmq::message_t update;
                zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));
                char* current = (char*)update.data();

                CharStruct newCharacter;
                char status; // 'd' for disconnect, 'c' for connect
                //Get client info
                int matches = sscanf_s(current, "%d %f %f %c", &(newCharacter.id), &(newCharacter.x), &(newCharacter.y), &status, 1);

                //While this loop is supposed to be for previously connected clients, a new one might slip in. Set them up!
                if (newCharacter.id == -1 && status == 'c') {
                    newCharacter.id = numCharacters;
                    //Only if we have room
                    if (numCharacters < 10) {
                        characters[numCharacters] = newCharacter;
                        numClients++;
                        numCharacters++;
                    }
                }
                //If it is a client that is disconnecting, set their ID as negative, so that other clients know not to render them
                else if (status == 'd' && numClients != 0 && newCharacter.id >= 0 && newCharacter.id < 10) {
                    characters[newCharacter.id].id = (characters[newCharacter.id].id + 1) * -1;
                    numClients--;
                }
                else if (status == 'c') { //If it's a returning client, update it's position only.
                    characters[newCharacter.id].x = newCharacter.x;
                    characters[newCharacter.id].y = newCharacter.y;
                }
                //Send a response with the character's new ID
                char response[MESSAGE_LIMIT];
                sprintf_s(response, "%d", newCharacter.id);
                zmq::message_t reply(strlen(response) + 1);
                memcpy(reply.data(), response, strlen(response) + 1);
                repSocket.send(reply, zmq::send_flags::none);
            }

            //This is where we check for new users explicitly, though it's also useful for getting our first user.
            //BUT if a new client slipped into the previous loop, it's possible this is also a returning client
            zmq::message_t init;
            zmq::recv_result_t received(repSocket.recv(init, zmq::recv_flags::dontwait));

            //If we found one, set it up
            if ((received.has_value() && (EAGAIN != received.value()))) {
                char* current = (char*)init.data();
                CharStruct newCharacter;
                char status;
                int matches = sscanf_s(current, "%d %f %f %c", &(newCharacter.id), &(newCharacter.x), &(newCharacter.y), &status, 1);
                if (newCharacter.id == -1 && status == 'c') {
                    newCharacter.id = numCharacters;
                    if (numCharacters < 10) {
                        characters[numCharacters] = newCharacter;
                        numClients++;
                        numCharacters++;
                    }
                }
                //If it is a client that is disconnecting, remove it from the array
                else if (status == 'd' && numClients != 0) {
                    characters[newCharacter.id].id = (characters[newCharacter.id].id + 1) * -1;
                    numClients--;
                }
                else if (status == 'c') { //Just in case another user was very fast and we got booted to this section.
                    characters[newCharacter.id].x = newCharacter.x;
                    characters[newCharacter.id].y = newCharacter.y;
                }
                //Send a response with the character's new (updated) ID
                char response[MESSAGE_LIMIT];
                sprintf_s(response, "%d", newCharacter.id);
                zmq::message_t reply(strlen(response) + 1);
                memcpy(reply.data(), response, strlen(response) + 1);
                repSocket.send(reply, zmq::send_flags::none);
            }


            //Done processing character updates, return platforms and characters
            if (numClients > 0) {

                //TODO: Cmbine these strings?

                //Construct platform position string
                char platRtnString[MESSAGE_LIMIT] = "";
                for (MovingPlatform *i : movings) {
                    char platString[MESSAGE_LIMIT];
                    sprintf_s(platString, "%f %f ", i->getPosition().x, i->getPosition().y);
                    strcat_s(platRtnString, platString);
                }
                //Send platform information
                zmq::message_t sendPlatforms(strlen(platRtnString) + 1);
                memcpy(sendPlatforms.data(), platRtnString, strlen(platRtnString) + 1);
                pubSocket.send(sendPlatforms, zmq::send_flags::none);

                //Construct character position string
                char charRtnString[MESSAGE_LIMIT] = "";
                for (int i = 0; i < numCharacters; i++) {
                    char charString[MESSAGE_LIMIT];
                    sprintf_s(charString, "%d %f %f ", characters[i].id, characters[i].x, characters[i].y);
                    strcat_s(charRtnString, charString);
                }
                //Send character information
                zmq::message_t sendCharacters(strlen(charRtnString) + 1);
                memcpy(sendCharacters.data(), charRtnString, strlen(charRtnString) + 1);
                pubSocket.send(sendCharacters, zmq::send_flags::none);
            }
        }
    }
    return EXIT_SUCCESS;
}