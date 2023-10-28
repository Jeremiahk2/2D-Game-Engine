#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <list>
#include <iostream>
#include "Character.h"
#include "Platform.h"

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
    std::list<GameObject*> staticObjects;

    std::list<std::shared_ptr<GameObject>> nonStaticObjects;

    std::list<GameObject*> collidables;

    std::list<GameObject*> drawables;

    std::map<int, std::shared_ptr<GameObject>> templates;

    GameObject* character;

    /**
    * Whether or not the window uses proportional scaling.
    */
    bool isProportional = true;

    std::mutex *innerMutex;




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
    bool checkCollisions(GameObject* collides);

    /**
    * Add a platform to the window. By default it is added to the list of platforms and collidables.
    */
    void addGameObject(GameObject* object);

    /**
    * Add the playable character to the window. Only one character is supported.
    */
    void addPlayableObject(GameObject* character);

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

    std::list<GameObject*>* getStaticObjects();

    /**
    * Return the list of platforms (realPlatforms) in the window.
    * @param n the number of real platforms in the window.
    */
    std::list<std::shared_ptr<GameObject>>* getNonstaticObjects();

    /**
    * return a pointer to the character.
    */
    GameObject* getPlayableObject();

    /**
    * Update the characters using a string that contains information about all of the updated characters.
    */
    void updateNonStatic(std::string updates);

    void addTemplate(std::shared_ptr<GameObject> templateObject);
};

#endif