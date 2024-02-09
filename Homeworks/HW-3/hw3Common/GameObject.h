#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#define OBJECT 0

class GameObject {

private:
    bool stationary = true;

    bool collidable = false;

    bool drawable = false;

public:

    static const int objectType = 0;

	GameObject(bool stationary, bool collidable, bool drawable);

    /**
    * Check if the object is static (Doesn't move or doesn't get send from the server)
    */
    bool isStatic();

    /**
    * Bool value for if it is drawable (inherits sf::drawable)
    */
    bool isDrawable();

    /**
    * Check if the object is collidable (inherits sf::Shape or sf::Shape)
    */
    bool isCollidable();

    /**
    * Set the collidable status of the object. 
    */
    void setCollidable(bool collidable);

    /**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
    virtual std::string toString() = 0;

    /**
    * Creates a new object using the string provided. To work with GameWindow, this should match toString.
    */
    virtual std::shared_ptr<GameObject> constructSelf(std::string self) = 0;

    /**
    * Creates a blank GameObject. Useful for when you need a copy of the object without knowing what type it is.
    */
    virtual std::shared_ptr<GameObject> makeTemplate() = 0;

    /**
    * Return the type of the class. Type should be a static const variable so that it is constant across all versions.
    * type should be unique for each version of GameObject that you use with GameWindow.
    */
    virtual int getObjectType() = 0;
};

#endif
