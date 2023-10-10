#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
//Correct version

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <list>
#include <thread>
#include <iostream>
#include "Character.h"
#include "CBox.h"
#include "Platform.h"

using namespace std;

/**
* GameWindow is a class that handles collisions and rendering the window.
* You can create a list of collidables, assign a character to the window, and a list of platforms.
* Then the methods will handle collision detection automatically with CheckCollisions.
*/
class GameWindow : public sf::RenderWindow {

private:

    /**
    * The list of platform pointers that need to be drawn on screen.
    */
    list<Platform*> platforms;

    Platform realPlatforms[10];

    /**
    * The list of CBoxes that need to be checked each cycle for collisions
    */
    list<CBox> collisions;

    /**
    * The character that is being checked for collisions with the CBoxes
    */
    Character* character;

    Character allCharacters[10];

    int numCharacters = 0;

    /**
    * Whether or not the window uses proportional scaling.
    */
    bool isProportional = true;

    int numPlatforms = 0;

    sf::Texture charTexture;




public:
    /**
    Empty constructor. Same as RenderWindow
    */
    GameWindow();

    /**
    Check collisions of all collidables versus the character.
    @param collides Returns the CBox that the character collided with (stationary platforms take precedence)
    I.E if a player is colliding with both a moving platform and a stationary platform, the stationary platform will be returned here.
    @return boolean value for if a collision was found.
    */
    bool checkCollisions(CBox* collides);

    /**
    * Add a platform to the window. By default it is added to the list of platforms and collidables.
    */
    void addPlatform(Platform* drawing, bool isMoving);

    /**
    * Add the playable character to the window. Only one character is supported.
    */
    void addCharacter(Character* character);

    /**
    * Update the window by clearing the window, drawing each object, and then displaying it.
    */
    void update();

    /**
    * Checks the mode of the window. True for proportional, false if not.
    */
    bool checkMode();

    /**
    * Change the scaling of the window. If it is currently constant, it will change to proportional, and vice versa.
    */
    void changeScaling();

    /**
    * If the current view is constant, this will resize the view to fit the new size
    */
    void handleResize(sf::Event event);

    /**
    * return a pointer to the character.
    */
    Character* getCharacter();

    Platform* getPlatforms(int* n);

    void updateCharacters(char *newChars);
};

#endif