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

/**
 * Wrapper function because threads can't take pointers to member functions.
 */
void run_moving(MovingThread *fe)
{
    fe->run();
}
//TODO: Combine these?
/**
* Run the CThread
*/
void run_cthread(CThread *fe) {
    fe->run();
}

int main() {

    // Create a window with the same pixel depth as the desktop, with 144 frames per second.
    GameWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(800, 600, desktop.bitsPerPixel), "Window", sf::Style::Default);

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


    //Create playable character and add it to the window as the playable object
    Character character;
    character.setSize(sf::Vector2f(30.f, 30.f));
    character.setFillColor(sf::Color::White);
    character.setPosition(startPlatform.getPosition());
    character.setOrigin(0.f, 30.f);
    character.setSpeed(CHAR_SPEED);
    character.setGravity(GRAV_SPEED);
    window.addCharacter(&character);

    //Set the bounds of the moving platforms
    vertMoving.setBounds(vertMoving.getStartPos().y, 200);
    moving.setBounds(startPlatform.getGlobalBounds().left + startPlatform.getGlobalBounds().width, endPlatform.getGlobalBounds().left - startPlatform.getGlobalBounds().width);

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

    //Draw everything that has been added to the window
    window.update();

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
    Timeline CTime(&global, TIC);


    MovingThread mthread(&MPTime, &stopped, 0, NULL, &m, &cv, &movings);

    CThread cthread(&upPressed,&window, &CTime, &stopped, &m, &cv);
    std::thread first(run_moving, &mthread);
    std::thread second(run_cthread, &cthread);

    //The amount of frames that a jump will take place.
    double jumpTime = JUMP_TIME;

    int64_t tic = 0;
    int64_t currentTic = 0;
    double scale = 1.0;
    double ticLength;
    //Begin main game loop
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
                second.join();
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
        if (currentTic > tic) {
            CBox collision;
            //Need to recalculate character speed in case scale changed.
            double charSpeed = (double)character.getSpeed().x * (double)ticLength * (double)(currentTic - tic);

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