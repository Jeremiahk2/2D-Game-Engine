#include "MovingPlatform.h"
#include "Character.h"
#include "CBox.h"
#include "GameWindow.h"
#include "CThread.h"
#include "ReqThread.h"
#include "SubThread.h"
//Correct version

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

//Speed is calculated in pixels per second.
#define PLAT_SPEED 100.f

#define PLAT_TIC 16

#define JUMP_SPEED 420.f

#define JUMP_TIME .5

#define TIC 8 //Setting this to 8 seems to produce optimal behavior. At least on my machine. 16 and 32 both work but they don't look very good. 4 usually results in 8 behavior anyway.
/**
* Run the CThread
*/
void run_cthread(CThread *fe) {
    fe->run();
}
//
//void run_reqthread(ReqThread* fe) {
//    fe->run();
//}
//void run_subthread(SubThread* fe) {
//    fe->run();
//}


int main() {

    //Setup window and add character.
    GameWindow window;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(800, 600, desktop.bitsPerPixel), "Window", sf::Style::Default);
    window.setActive(false);

    //Create StartPlatform and add it to the window
    Platform startPlatform;
    startPlatform.setSize(sf::Vector2f(100.f, 15.f));
    startPlatform.setFillColor(sf::Color(100, 0, 0));
    startPlatform.setPosition(sf::Vector2f(50.f, 500.f));
    window.addGameObject(&startPlatform);

    //Create endPlatform and add it to the window
    Platform endPlatform;
    endPlatform.setSize(sf::Vector2f(100.f, 15.f));
    endPlatform.setFillColor(sf::Color(218, 165, 32));
    endPlatform.setPosition(sf::Vector2f(500.f, 500.f));
    window.addGameObject(&endPlatform);

    //Create headBonk platform (for testing jump) and add it to the window
    Platform headBonk;
    headBonk.setSize(sf::Vector2f(100.f, 15.f));
    headBonk.setFillColor(sf::Color::Blue);
    headBonk.setPosition(500.f, 440.f);
    window.addGameObject(&headBonk);

    //Create playable character and add it to the window as the playable object
    Character character;
    character.setPosition(100.f, startPlatform.getPosition().y - character.getGlobalBounds().height - 1.f);
    /*character.setPosition(100.f, 100.f);*/
    window.addPlayableObject(&character);

    window.addTemplate(headBonk.makeTemplate());
    window.addTemplate(character.makeTemplate());
    window.addTemplate(std::shared_ptr<MovingPlatform>(new MovingPlatform));

    //END SETTING UP GAME OBJECTS

    //Set up timing variables
    int64_t tic = 0;
    int64_t currentTic = 0;
    float scale = 1.0;
    float ticLength;
    float jumpTime = JUMP_TIME;

    //Set up necessary thread vairables
    std::mutex mutex;
    bool upPressed = false;
    bool busy = true;
    bool stopped = false;
    std::condition_variable cv;

    //Set up Timelines
    Timeline global;
    Timeline FrameTime(&global, TIC);
    Timeline CTime(&global, TIC);

    //Start collision detection thread
    CThread cthread(&upPressed, &window, &CTime, &stopped, &mutex, &cv, &busy);
    std::thread first(run_cthread, &cthread);

    while (window.isOpen()) {

        ticLength = FrameTime.getRealTicLength();
        currentTic = FrameTime.getTime();

        sf::Event event;
        if (currentTic > tic) {

            int polls = 0;
            while (window.pollEvent(event)) {
                if (polls > 2) {
                    //busy = false; //We've been in the event loop for too long, probably due to a resize or something similar. Allow our thread to continue.
                }
                if (event.type == sf::Event::Closed) {
                    stopped = true;
                    //Need to notify all so they can stop
                    cv.notify_all();
                    first.join();
                    //second.join();
                    //third.join();
                    window.close();

                }
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q)) {
                    window.changeScaling();
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
                polls++;
            }
            //Out of the event loop, need to sync up with thread again.

            //END EVENT CHECKS
            //Need to recalculate character speed in case scale changed.
            float charSpeed = (float)character.getSpeed().x * (float)ticLength * (float)(currentTic - tic);

            GameObject* collision = nullptr;
            //Handle left input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && window.hasFocus())
            {
                std::lock_guard<std::mutex> lock(mutex);
                //Move left
                character.move(-1 * charSpeed, 0.f);

                //Check for collisions
                if (window.checkCollisions(&collision)) {
                    //If the collided platform is not moving, just correct the position of Character back.
                    if (collision->getObjectType() != MovingPlatform::objectType) {
                        character.move(charSpeed, 0.f);
                    }
                    //if the collided platform is moving, move the character back AND move them along with the platform.
                    else {
                        MovingPlatform *temp = (MovingPlatform *)collision;
                        //Convert plat's speed to pixels per tic
                        float platSpeed = (float)temp->getSpeedValue() * (float)ticLength * (float)(currentTic - tic);

                        if (temp->getMovementType()) {
                            character.move(charSpeed + platSpeed, 0);
                        }
                        else {
                            character.move(charSpeed, 0);
                        }
                    }
                }
            }

            //Handle right input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && window.hasFocus())
            {
                std::lock_guard<std::mutex> lock(mutex);
                // Move Right
                character.move(charSpeed, 0.f);

                //Check collisions.
                if (window.checkCollisions(&collision)) {
                    //If the collided object is not moving, just correct the position of the character back.
                    if (collision->getObjectType() != MovingPlatform::objectType) {
                        character.move(-1 * charSpeed, 0.f);
                    }
                    //If it was moving, the move it back AND along with the platform's speed.
                    else {
                        MovingPlatform *temp = (MovingPlatform*)collision;
                        float platSpeed = (float)temp->getSpeedValue() * (float)ticLength * (float)(currentTic - tic);
                        if (temp->getMovementType()) {
                            character.move(-1 * charSpeed + platSpeed, 0);
                        }
                        else {
                            character.move(-1 * charSpeed, 0);
                        }
                    }
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && window.hasFocus())
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (upPressed) {
                    upPressed = false;
                    character.setJumping(true);
                }
            }

            //If the character is currently jumping, move them up and check for collisions.
            float frameJump = JUMP_SPEED * (float)ticLength * (float)(currentTic - tic);
            
            if (character.isJumping()) {
                std::lock_guard<std::mutex> lock(mutex);
                character.move(0, -1 * frameJump);
                jumpTime -= (float)ticLength * (float)(currentTic - tic);
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
            tic = currentTic;
        }
        /*tic = currentTic;*/
    }

    return EXIT_SUCCESS;
}