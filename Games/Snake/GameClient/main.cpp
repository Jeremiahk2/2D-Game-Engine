#include "MovingPlatform.h"
#include "Character.h"
#include "GameWindow.h"
#include "CThread.h"
#include "DeathZone.h"
#include "SideBound.h"
#include <v8.h>
#include "v8helpers.h"
#include "ScriptManager.h"
#include <cstdio>
#include <libplatform/libplatform.h>
#define V8_COMPRESS_POINTERS 1
#define V8_31BIT_SMIS_ON_64BIT_ARCH 1
#define DV8_ENABLE_SANDBOX 1
#define OBJECT 0
#define CHARACTER 1
#define PLATFORM 2
#define MOVING 3
#define SPAWNPOINT 4
#define DEATHZONE 5
#define SIDEBOUND 6
#define EVENT 7

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

//Speed is calculated in pixels per second.
#define PLAT_SPEED 100.f

#define PLAT_TIC 16

#define JUMP_SPEED 420.f

#define JUMP_TIME .5

#define TIC 75 
/**
* Run the CThread
*/
void run_cthread(CThread *fe) {
    fe->run();
}


int main(int argc, char **argv) {

        GameWindow window;

        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        window.create(sf::VideoMode(800, 600), "Window", sf::Style::Default);
        window.setView(sf::View(sf::FloatRect(0, 0, 860, 645)));
        window.setActive(false);

        //Create StartPlatform and add it to the window
        Platform bottom;
        bottom.setSize(sf::Vector2f(800, 10.f));
        bottom.setFillColor(sf::Color(100, 0, 0));
        bottom.setPosition(sf::Vector2f(0, 590));
        window.addGameObject(&bottom);

        Platform right;
        right.setSize(sf::Vector2f(10, 580));
        right.setFillColor(sf::Color(100, 0, 0));
        right.setPosition(sf::Vector2f(790, 10));
        window.addGameObject(&right);

        Platform left;
        left.setSize(sf::Vector2f(10, 580));
        left.setFillColor(sf::Color(100, 0, 0));
        left.setPosition(sf::Vector2f(0, 10));
        window.addGameObject(&left);

        Platform top;
        top.setSize(sf::Vector2f(800, 10.f));
        top.setFillColor(sf::Color(100, 0, 0));
        top.setPosition(sf::Vector2f(0, 0));
        window.addGameObject(&top);

        Platform personal;
        personal.setSize(sf::Vector2f(150.f, 40.f));
        personal.setFillColor(sf::Color::Transparent);
        personal.setPosition(sf::Vector2f(250.f, 600.f));
        

        Character character;
        character.setPosition(10, 10);
        character.setSpawnPoint(SpawnPoint(character.getPosition()));
        character.setConnecting(1);
        window.addPlayableObject(&character);
        //Set up the list of occupied squares.
        for (int i = 0; i < 780 / character.getSize().x; i++) {
            for (int j = 0; j < 580 / character.getSize().y; j++) {
                //Skip the first square (Character is occupying it)
                if (!(i == 0 && j == 0)) {
                    character.unoccupied.push_back(sf::Vector2f(i * character.getSize().x + 10, j * character.getSize().y + 10));
                }
            }
        }

        //Get initial apple position
        srand(time(NULL));
        int appleIndex = rand() % character.unoccupied.size();
        int count = 0;
        sf::Vector2f newPosition;
        std::list<sf::Vector2f>::iterator it = character.unoccupied.begin();
        for (sf::Vector2f i : character.unoccupied) {
            if (count == appleIndex) {
                newPosition = i;
            }
            it++;
            count++;
        }
        //Change the apple's position to the generated one.
        character.apple->setPosition(newPosition);
        window.addGameObject(character.apple);
        character.apple->setFillColor(sf::Color::Red);
        character.apple->setSize(sf::Vector2f(CHAR_SPEED, CHAR_SPEED));
        character.apple->setOutlineThickness(-1.f);
        character.apple->setOutlineColor(sf::Color::Black);

        //Add templates
        window.addTemplate(bottom.makeTemplate());
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
        Timeline globalTime;
        Timeline FrameTime(&globalTime, TIC);
        Timeline CTime(&globalTime, TIC);

        EventManager eventManager(&window, &globalTime);


        //Start collision detection thread
        CThread cthread(&upPressed, &window, &CTime, &stopped, &mutex, &cv, &busy, &eventManager);
        std::thread first(run_cthread, &cthread);
        int lastLeft = 0;
        int lastRight = 0;
        int lastStop = 0;
        bool leftPressed = false;
        bool rightPressed = false;

        std::string type("stop");
        std::list<std::string> types;
        types.push_back(type);
        eventManager.registerEvent(types, new StopHandler());

        type = "input";
        types.clear();
        types.push_back(type);
        eventManager.registerEvent(types, new MovementHandler());

        Event m;
        m.order = 1;
        m.time = FrameTime.convertGlobal(currentTic);
        m.type = std::string("input");

        Event::variant characterVariant;
        characterVariant.m_Type = Event::variant::TYPE_GAMEOBJECT;
        characterVariant.m_asGameObject = &character;
        m.parameters.insert({ "character", characterVariant });

        Event::variant directionVariant;
        directionVariant.m_Type = Event::variant::TYPE_INT;
        directionVariant.m_asInt = MovementHandler::DIRECTION::UP;
        m.parameters.insert({ "direction", directionVariant });
        int numInputs = 0;

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
                    window.setActive(true);
                    window.close();

                }
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q)) {
                    window.changeScaling();
                }

                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)) {
                    m.time = FrameTime.convertGlobal(currentTic + ++numInputs);
                    directionVariant.m_asInt = MovementHandler::DIRECTION::UP;
                    m.parameters.insert_or_assign( "direction", directionVariant );
                    eventManager.raise(m);
                }
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)) {
                    m.time = FrameTime.convertGlobal(currentTic + ++numInputs);
                    directionVariant.m_asInt = MovementHandler::DIRECTION::LEFT;
                    m.parameters.insert_or_assign( "direction", directionVariant );
                    eventManager.raise(m);
                }
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)) {
                    m.time = FrameTime.convertGlobal(currentTic + ++numInputs);
                    directionVariant.m_asInt = MovementHandler::DIRECTION::DOWN;
                    m.parameters.insert_or_assign("direction", directionVariant );
                    eventManager.raise(m);
                }
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)) {
                    m.time = FrameTime.convertGlobal(currentTic + ++numInputs);
                    directionVariant.m_asInt = MovementHandler::DIRECTION::RIGHT;
                    m.parameters.insert_or_assign( "direction", directionVariant );
                    eventManager.raise(m);
                }
                if (event.type == sf::Event::Resized)
                {
                    window.handleResize(event);
                }
            }
            if (currentTic > tic) {
                numInputs = 0;
                tic = currentTic;
            }
        }
    return EXIT_SUCCESS;
}