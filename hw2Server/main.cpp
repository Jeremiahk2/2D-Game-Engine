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

struct PlatStruct {
    Platform plat;
};

int main() {
    // Create a window with the same pixel depth as the desktop, with 144 frames per second.
    GameWindow window;

    //Create StartPlatform and add it to the window
    Platform startPlatform;
    startPlatform.setSize(sf::Vector2f(100.f, 15.f));
    startPlatform.setFillColor(sf::Color(100, 0, 0));
    startPlatform.setPosition(sf::Vector2f(150.f - startPlatform.getSize().x, 500.f));
    window.addPlatform(&startPlatform, false);

    //Create MovingPlatform and add it to the window
    MovingPlatform moving(PLAT_SPEED, 1, startPlatform.getGlobalBounds().left + startPlatform.getGlobalBounds().width, 500.f);
    moving.setSize(sf::Vector2f(100.f, 15.f));
    moving.setFillColor(sf::Color(100, 250, 50));
    window.addPlatform(&moving, true);

    //Create endPlatform and add it to the window
    Platform endPlatform;
    endPlatform.setSize(sf::Vector2f(100.f, 15.f));
    endPlatform.setFillColor(sf::Color(218, 165, 32));
    endPlatform.setPosition(sf::Vector2f(400.f + endPlatform.getSize().x, 500.f));
    window.addPlatform(&endPlatform, false);

    MovingPlatform vertMoving(PLAT_SPEED, false, endPlatform.getPosition().x + endPlatform.getSize().x, 500.f);
    vertMoving.setSize(sf::Vector2f(50.f, 15.f));
    vertMoving.setFillColor(sf::Color::Magenta);
    window.addPlatform(&vertMoving, true);

    //Create headBonk platform (for testing jump) and add it to the window
    Platform headBonk;
    headBonk.setSize(sf::Vector2f(100.f, 15.f));
    headBonk.setFillColor(sf::Color::Blue);
    headBonk.setPosition(endPlatform.getPosition().x, endPlatform.getPosition().y - 60);
    window.addPlatform(&headBonk, false);

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

    Character characters[10];
    int numCharacters = 0;

    //Begin main update loop
    while (true) {

        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();
        if (currentTic > tic) {
            //We are expecting updates from all of our users, but a new one might slip in too.
            for (int i = 0; i < numCharacters; i++) {
                zmq::message_t update;
                zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));
                Character* current = (Character *)update.data();
                //If it's a new client mixed in here, set it up.
                if (current->getID() == -1) {
                    current->setID(numCharacters);
                    characters[numCharacters] = *current;
                    numCharacters++;
                }
                //Else, replace it's position in the list (Updates it's position);
                else {
                    characters[current->getID()] = *current;
                }
                //Confirmation reply
                zmq::message_t reply(20);
                memcpy(reply.data(), "Connection Received", 20);
                repSocket.send(reply, zmq::send_flags::none);
            }

            //Check for incoming users. This will probably only occur for the first user.
            zmq::message_t init;
            zmq::recv_result_t received(repSocket.recv(init, zmq::recv_flags::dontwait));

            //If we found one, set it up
            if ((received.has_value() && (EAGAIN != received.value()))) {
                Character* current = (Character*)init.data();
                Character newCharacter;
                newCharacter.setOrigin(current->getOrigin());
                //std::cout << "Ran4\n";
                //newCharacter.setTexture(current->getTexture());
                newCharacter.setSize(current->getSize());
                newCharacter.setPosition(current->getPosition());
                newCharacter.setFillColor(current->getFillColor());
                newCharacter.setID(numCharacters);
                characters[numCharacters] = newCharacter;
                numCharacters++;

                //Confirmation reply
                zmq::message_t reply(20);
                memcpy(reply.data(), "Connection Received", 20);
                repSocket.send(reply, zmq::send_flags::none);
            }

            //Done processing character updates, return platforms and characters
            //TODO: NEED TO BE NOTIFIED BY MTHREAD BEFORE DOING THIS
            int numPlatforms;
            Platform* rtnPlatforms = window.getPlatforms(&numPlatforms);
            if (numCharacters > 0) {
                //Send platforms
                std::cout << "Ran\n";
                zmq::message_t sendPlatforms(numPlatforms * sizeof(Platform));
                memcpy(sendPlatforms.data(), rtnPlatforms, numPlatforms * sizeof(Platform));
                pubSocket.send(sendPlatforms, zmq::send_flags::none);
                std::cout << "Ran2\n" << numPlatforms * sizeof(Platform) << std::endl; //Doesn't account for MOVING platform Use struct with both

                //Send characters
                zmq::message_t sendCharacters(numCharacters * sizeof(Character));
                memcpy(sendCharacters.data(), characters, numCharacters * sizeof(Character));
                pubSocket.send(sendCharacters, zmq::send_flags::none);
            }
        }
    }
    return EXIT_SUCCESS;
}