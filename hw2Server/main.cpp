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
#include "CThread.h"

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

#define PLAT_TIC 16

#define JUMP_SPEED 420.f

#define JUMP_TIME .5

#define TIC 16

#define MESSAGE_LIMIT 1024

/**
 * Wrapper function because threads can't take pointers to member functions.
 */
void run_moving(MovingThread* fe)
{
    fe->run();
}
//TODO: Combine these?
/**
* Run the CThread
*/
void run_cthread(CThread* fe) {
    fe->run();
}

struct CharStruct {
    int id;
    float x;
    float y;
};

int main() {

    //Create StartPlatform and add it to the window
    Platform startPlatform;
    startPlatform.setSize(sf::Vector2f(100.f, 15.f));
    startPlatform.setFillColor(sf::Color(100, 0, 0));
    startPlatform.setPosition(sf::Vector2f(150.f - startPlatform.getSize().x, 500.f));

    //Create MovingPlatform and add it to the window
    MovingPlatform moving(PLAT_SPEED, 1, startPlatform.getGlobalBounds().left + startPlatform.getGlobalBounds().width, 500.f);
    moving.setSize(sf::Vector2f(100.f, 15.f));
    moving.setFillColor(sf::Color(100, 250, 50));

    //Create endPlatform and add it to the window
    Platform endPlatform;
    endPlatform.setSize(sf::Vector2f(100.f, 15.f));
    endPlatform.setFillColor(sf::Color(218, 165, 32));
    endPlatform.setPosition(sf::Vector2f(400.f + endPlatform.getSize().x, 500.f));

    MovingPlatform vertMoving(PLAT_SPEED, false, endPlatform.getPosition().x + endPlatform.getSize().x, 500.f);
    vertMoving.setSize(sf::Vector2f(50.f, 15.f));
    vertMoving.setFillColor(sf::Color::Magenta);

    //Create headBonk platform (for testing jump) and add it to the window
    Platform headBonk;
    headBonk.setSize(sf::Vector2f(100.f, 15.f));
    headBonk.setFillColor(sf::Color::Blue);
    headBonk.setPosition(endPlatform.getPosition().x, endPlatform.getPosition().y - 60);

    //Set the bounds of the moving platforms
    vertMoving.setBounds(vertMoving.getStartPos().y, 200);
    moving.setBounds(startPlatform.getGlobalBounds().left + startPlatform.getGlobalBounds().width, endPlatform.getGlobalBounds().left - startPlatform.getGlobalBounds().width);



    //Add Moving Platforms to the list.
    list<MovingPlatform*> movings;
    movings.push_front(&moving);
    movings.push_front(&vertMoving);

    bool stopped = false;

    std::mutex m;
    std::condition_variable cv;
    bool upPressed = false;

    //This should allow altering all timelines through global (pausing).
    Timeline global;
    //MPTime and CTime need to be the same tic atm. Framtime can be different (though not too low)
    Timeline MPTime(&global, TIC);
    Timeline FrameTime(&global, TIC);


    MovingThread mthread(&MPTime, &stopped, 0, NULL, &m, &cv, &movings);

    std::thread first(run_moving, &mthread);

    int64_t tic = 0;
    int64_t currentTic = 0;
    double scale = 1.0;
    double ticLength;

    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    zmq::socket_t pubSocket(context, zmq::socket_type::pub);
    repSocket.bind("tcp://localhost:5555");
    pubSocket.bind("tcp://localhost:5556");

    CharStruct characters[10];
    int numClients = 0;
    int numCharacters = 0;

    //Begin main update loop
    while (true) {

        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();
        if (currentTic > tic) {

            //We are expecting updates from all of our users, but a new one might slip in too.
            for (int i = 0; i < numClients; i++) {
                zmq::message_t update;
                zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));
                char* current = (char*)update.data();
                CharStruct newCharacter;
                char status;
                int matches = sscanf_s(current, "%d %f %f %c", &(newCharacter.id), &(newCharacter.x), &(newCharacter.y), &status, 1);

                //The reply we will send to the user

                //If it's a new client, who is connecting for the first time, set it up in the array.
                if (newCharacter.id == -1 && status == 'c') {
                    newCharacter.id = numClients;
                    characters[numClients] = newCharacter;
                    numClients++;
                    numCharacters++;

                    //Send a response with the character's new ID
                    char response[MESSAGE_LIMIT];
                    sprintf_s(response, "%d", newCharacter.id);
                    zmq::message_t reply(strlen(response) + 1);
                    memcpy(reply.data(), response, strlen(response) + 1);
                    repSocket.send(reply, zmq::send_flags::none);
                }
                //If it is a client that is disconnecting, remove it from the array
                else if (status == 'd' && numClients != 0 && newCharacter.id >= 0 && newCharacter.id < 9) {
                    characters[newCharacter.id].id = (characters[newCharacter.id].id + 1) * -1;
                    numClients--;

                    //Send a response with the character's ID. Should be the same.
                    char response[MESSAGE_LIMIT];
                    sprintf_s(response, "%d", newCharacter.id);
                    zmq::message_t reply(strlen(response) + 1);
                    memcpy(reply.data(), response, strlen(response) + 1);
                    repSocket.send(reply, zmq::send_flags::none);
                }
                else if (status == 'c') { //If it's a returning client, update it's position only.
                    characters[newCharacter.id].x = newCharacter.x;
                    characters[newCharacter.id].y = newCharacter.y;

                    //Send a response with the character's ID. Should be the same.
                    char response[MESSAGE_LIMIT];
                    sprintf_s(response, "%d", newCharacter.id);
                    zmq::message_t reply(strlen(response) + 1);
                    memcpy(reply.data(), response, strlen(response) + 1);
                    repSocket.send(reply, zmq::send_flags::none);
                }
            }

            //Check for incoming users. This will probably only occur for the first user.
            zmq::message_t init;
            zmq::recv_result_t received(repSocket.recv(init, zmq::recv_flags::dontwait));

            //If we found one, set it up
            if ((received.has_value() && (EAGAIN != received.value()))) {
                char* current = (char*)init.data();
                CharStruct newCharacter;
                char status;
                int matches = sscanf_s(current, "%d %f %f %c", &(newCharacter.id), &(newCharacter.x), &(newCharacter.y), &status, 1);
                if (newCharacter.id == -1 && status == 'c') {
                    newCharacter.id = numClients;
                    characters[numClients] = newCharacter;
                    numClients++;
                    numCharacters++;

                    //Send a response with the character's new ID
                    char response[MESSAGE_LIMIT];
                    sprintf_s(response, "%d", newCharacter.id);
                    zmq::message_t reply(strlen(response) + 1);
                    memcpy(reply.data(), response, strlen(response) + 1);
                    repSocket.send(reply, zmq::send_flags::none);
                }
                //If it is a client that is disconnecting, remove it from the array
                else if (status == 'd' && numClients != 0) {
                    characters[newCharacter.id].id = (characters[newCharacter.id].id + 1) * -1;
                    numClients--;

                    //Send a response with the character's new ID
                    char response[MESSAGE_LIMIT];
                    sprintf_s(response, "%d", newCharacter.id);
                    zmq::message_t reply(strlen(response) + 1);
                    memcpy(reply.data(), response, strlen(response) + 1);
                    repSocket.send(reply, zmq::send_flags::none);
                }
                else if (status == 'c') { //Just in case another user was very fast and somehow slipped in here.
                    characters[newCharacter.id].x = newCharacter.x;
                    characters[newCharacter.id].y = newCharacter.y;

                    //Send a response with the character's new ID
                    char response[MESSAGE_LIMIT];
                    sprintf_s(response, "%d", newCharacter.id);
                    zmq::message_t reply(strlen(response) + 1);
                    memcpy(reply.data(), response, strlen(response) + 1);
                    repSocket.send(reply, zmq::send_flags::none);
                }
            }


            //Done processing character updates, return platforms and characters
            //TODO: NEED TO BE NOTIFIED BY MTHREAD BEFORE DOING THIS
            if (numClients > 0) {
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