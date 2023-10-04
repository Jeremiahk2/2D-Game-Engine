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
#include <list>
#include <thread>
#include <zmq.hpp>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif
using namespace std;

/**
* KNOWN BUGS
* 1. Changing the scale to a lower scale at the precise moment that the 
* platform hits it's bounds will cause it to stop in place. This is probably because
* it's new speed is too slow to make it back through the bounded area.
* This can probably be fixed by updating the platform's position to the bound when it hits it.
* That way a speed change won't affect it.
* Moderately difficult to replicate
* 2. Very rare circumstance where if the player is on a vertical moving platform for a long time, they will fall through the platform eventually
* Very difficult to replicate (Random occurrence)
*/

//Test settings. Feel free to change these.
#define CHAR_SPEED 100.f

// #define GRAV_SPEED 160.f
#define GRAV_SPEED 160

//Speed is calculated in pixels per second.
#define PLAT_SPEED 100.f

#define PLAT_TIC 16

#define JUMP_SPEED 420.f

#define JUMP_TIME .5

//TESTS:
// 9/4/2023 4:26 PM
// Left alone for 30 minutes on 16 tic (at 1.0 scale) and nothing broke. My character remained on the horizontal moving platform.
// Left player alone for 24 minutes on 16 tic (at 1.0 scale) on the vertical platform and the player fell off at some point near the end of this period.
#define TIC 16 //Setting this to 4 or lower causes problems. CThread can't keep up and causes undefined behavior. Need to use mutexes or some equivalent to fix this. Though it's technically on the client.

//TODO: Combine these?
/**
* Run the CThread
*/
void run_cthread(CThread *fe) {
    fe->run();
}

int main() {

    //Create playable character and add it to the window as the playable object
    Character character;
    character.setSize(sf::Vector2f(30.f, 30.f));
    character.setFillColor(sf::Color::White);
    character.setOrigin(0.f, 30.f);
    character.setPosition(100.f, 100.f);

    /**
    ART FOR SANTA PROVIDED BY Marco Giorgini THROUGH OPENGAMEART.ORG
    License: CC0 (Public Domain) @ 12/28/21
    https://opengameart.org/content/santas-rats-issue-c64-sprites
    */
    sf::Texture charTexture;
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
    character.setTexture(&charTexture);

    //Setup window and add character.
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    GameWindow window;
    window.create(sf::VideoMode(800, 600, desktop.bitsPerPixel), "Window", sf::Style::Default);
    window.addCharacter(&character);


    //Setup timing stuff
    int64_t tic = 0;
    int64_t currentTic = 0;
    double scale = 1.0;
    double ticLength;

    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t reqSocket(context, zmq::socket_type::req);
    zmq::socket_t subSocket(context, zmq::socket_type::sub);

    //Connect
    reqSocket.connect("tcp://localhost:5555");
    subSocket.connect("tcp://localhost:5556");

    //This should allow altering all timelines through global (pausing).
    Timeline global;
    //MPTime and CTime need to be the same tic atm. Framtime can be different (though not too low)
    Timeline FrameTime(&global, TIC);
    
    //Bool for if the threads should stop
    bool stopped = false;

    //Set up necessary thread vairables
    std::mutex m;
    std::condition_variable cv;
    bool upPressed = false;

    Timeline CTime(&global, TIC);

    //Start collision detection
    CThread cthread(&upPressed, &window, &CTime, &stopped, &m, &cv);
    std::thread first(run_cthread, &cthread);

    Platform platforms[10];

    //Start main game loop
    while (window.isOpen()) {
        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                stopped = true;
                //Need to notify all so they can stop
                cv.notify_all();
                first.join();
                window.close();
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q)) {
                window.changeScaling();
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::P)) {
                if (global.isPaused()) {
                    global.unpause();
                }
                else {
                    global.pause();
                }
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Left)) {
                if (scale != .5) {
                    scale *= .5;
                    global.changeScale(scale);
                }
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Right)) {
                if (scale != 2.0) {
                    scale *= 2.0;
                    global.changeScale(scale);
                }
            }
            if (event.type == sf::Event::Resized)
            {
                window.handleResize(event);
            }
        }

        //Send updated character information to server
        zmq::message_t request(sizeof(Character));
        memcpy(request.data(), &character, sizeof(Character));
        reqSocket.send(request, zmq::send_flags::none);

        //Receive confirmation
        zmq::message_t reply;
        reqSocket.recv(reply, zmq::recv_flags::none);
        std::cout << "Ran2\n";

        //Receive updated platforms
        zmq::message_t newPlatforms;
        subSocket.recv(newPlatforms, zmq::recv_flags::none);
        std::cout << "Ran3\n";
            
        for (int i = 0; i < newPlatforms.size() / sizeof(Platform); i++) {
            platforms[i] = *(Platform*)(newPlatforms.data());
        }
        window.updatePlatforms(platforms, newPlatforms.size() / sizeof(Platform));


        //Receive updated characters
        zmq::message_t newCharacters;
        subSocket.recv(newCharacters, zmq::recv_flags::none);

        window.updateCharacters((Character*)newCharacters.data(), newCharacters.size() / sizeof(Character));

        //Update window visuals
        window.update();

        double jumpTime = JUMP_TIME;


        CBox collision;

        //Need to recalculate character speed in case scale changed.
        double charSpeed = (double)character.getSpeed().x * (double)ticLength * (double)(currentTic - tic);
        if (currentTic > tic) {
            //Handle left input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                //Move left
                character.move(-1 * charSpeed, 0.f);

                //Check for collisions
                if (window.checkCollisions(&collision)) {
                    //If the collided platform is not moving, just correct the position of Character back.
                    if (!collision.isMoving()) {
                        character.move(charSpeed, 0.f);
                    }
                    //if the collided platform is moving, move the character back AND move them along with the platform.
                    else {
                        MovingPlatform *temp = (MovingPlatform *)collision.getPlatform();
                        if (temp->getType()) {
                            character.move(charSpeed + temp->getLastMove().x, 0);
                        }
                        else {
                            character.move(charSpeed, 0);
                        }
                    }
                }
            }

            //Handle right input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                // Move Right
                character.move(charSpeed, 0.f);

                //Check collisions.
                if (window.checkCollisions(&collision)) {
                    //If the collided object is not moving, just correct the position of the character back.
                    if (!collision.isMoving()) {
                        character.move(-1 * charSpeed, 0.f);
                    }
                    //If it was moving, the move it back AND along with the platform's speed.
                    else {
                        MovingPlatform *temp = (MovingPlatform *)collision.getPlatform();
                        if (temp->getType()) {
                            character.move(-1 * charSpeed + temp->getLastMove().x, 0);
                        }
                        else {
                            character.move(-1 * charSpeed, 0);
                        }
                    }
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && upPressed)
            {
                upPressed = false;
                character.setJumping(true);
            }

            //If the character is currently jumping, move them up and check for collisions.
            double frameJump = JUMP_SPEED * (double)ticLength * (double)(currentTic - tic);
            if (character.isJumping()) {
                character.move(0, -1 * frameJump);
                jumpTime -= (double)ticLength * (double)(currentTic - tic);
                bool jumpCollides = window.checkCollisions(&collision);
                //Exit jumping if there are no more jump frames or if we collided with something.
                if (jumpTime <= 0 || jumpCollides) {
                    if (jumpCollides) {
                        character.move(0, frameJump);
                    }
                    character.setJumping(false);
                    jumpTime = JUMP_TIME;
                }
            }
            //Update the window's visuals.
            if (!stopped && !global.isPaused()) {
                //Wait for MThread to update. This creates a smoother looking ride, as the updates are consistently at the same spot.
                std::unique_lock<std::mutex> lock(m);
                cv.wait(lock);
                window.update();
            }
        }
        tic = currentTic;
    }
    return EXIT_SUCCESS;
}