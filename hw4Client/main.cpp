#include "MovingPlatform.h"
#include "Character.h"
#include "GameWindow.h"
#include "CThread.h"
#include "DeathZone.h"
#include "SideBound.h"

#define OBJECT 0
#define CHARACTER 1
#define PLATFORM 2
#define MOVING 3
#define SPAWNPOINT 4
#define DEATHZONE 5
#define SIDEBOUND 6
#define EVENT 7

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
    character.setSpawnPoint(SpawnPoint(character.getPosition()));
    character.setConnecting(1);
    window.addPlayableObject(&character);

    //Make a deadzone of height 30 that stretches across the bottom of the view.
    DeathZone dead(sf::Vector2f(window.getView().getSize().x * 2, 30.f), sf::Vector2f(0.f, window.getView().getSize().y - 30.f));
    window.addGameObject(&dead);
    //Create first SideBound
    sf::View view1 = window.getView();
    SideBound firstView(&window, view1);
    firstView.setPosition(500.f, 0.f);
    firstView.setSize(sf::Vector2f(15.f, (float)window.getSize().y));
    window.addGameObject(&firstView);

    sf::View view3 = window.getView();
    SideBound thirdView(&window, view3);
    thirdView.setPosition(character.getPosition());
    thirdView.setSize(sf::Vector2f(15.f, (float)window.getSize().y));
    window.addGameObject(&thirdView);

    //Create second SideBound
    sf::View view2 = window.getView();
    view2.setCenter(window.getView().getCenter().x + 450.f, window.getView().getCenter().y);
    SideBound secondView(&window, view2);
    secondView.setSize(sf::Vector2f(15.f, (float)window.getSize().y));
    secondView.setPosition(firstView.getPosition().x + firstView.getGlobalBounds().width + character.getGlobalBounds().width + 1.f, 0.f);
    window.addGameObject(&secondView);

    //Add templates
    window.addTemplate(headBonk.makeTemplate());
    window.addTemplate(character.makeTemplate());
    window.addTemplate(dead.makeTemplate());
    window.addTemplate(secondView.makeTemplate());
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
    EventManager eventManager;

    //Set up Timelines
    Timeline global;
    Timeline FrameTime(&global, TIC);
    Timeline CTime(&global, TIC);

    //Start collision detection thread
    CThread cthread(&upPressed, &window, &CTime, &stopped, &mutex, &cv, &busy, &eventManager);
    std::thread first(run_cthread, &cthread);

    while (window.isOpen()) {

        ticLength = global.getRealTicLength();
        currentTic = global.getTime();

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
                    window.setActive(true);
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

            Event m;
            m.type = std::string("movement");

            Event::variant windowVariant;
            windowVariant.m_Type = Event::variant::TYPE_GAMEWINDOW;
            windowVariant.m_asGameWindow = &window;

            Event::variant ticLengthVariant;
            ticLengthVariant.m_Type = Event::variant::TYPE_FLOAT;
            ticLengthVariant.m_asFloat = ticLength;

            Event::variant differentialVariant;
            differentialVariant.m_Type = Event::variant::TYPE_INT;
            differentialVariant.m_asInt = currentTic - tic;

            m.parameters.insert({ "window", windowVariant });
            m.parameters.insert({ "ticLength", ticLengthVariant });
            m.parameters.insert({ "differential", differentialVariant });
            m.type = std::string("movement");
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && window.hasFocus())
            {
                std::lock_guard<std::mutex> lock(mutex);
                Event::variant directionVariant;
                directionVariant.m_Type = Event::variant::TYPE_INT;
                directionVariant.m_asInt = MovementHandler::LEFT;
                m.parameters.insert({ "direction", directionVariant });
                eventManager.raise(m);
            }

            //Handle right input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && window.hasFocus())
            {
                std::lock_guard<std::mutex> lock(mutex);
                Event::variant directionVariant;
                directionVariant.m_Type = Event::variant::TYPE_INT;
                directionVariant.m_asInt = MovementHandler::RIGHT;
                m.parameters.insert({ "direction", directionVariant });
                eventManager.raise(m);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && window.hasFocus())
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (upPressed) {
                    upPressed = false;
                    character.setJumping(true);
                }
            }
            tic = currentTic;
        }
    }

    return EXIT_SUCCESS;
}