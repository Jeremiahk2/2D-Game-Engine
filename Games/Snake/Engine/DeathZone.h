#ifndef DEATHZONE_H
#define DEATHZONE_H
#include "GameObject.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class DeathZone : public GameObject, public sf::RectangleShape {
private:
public:
    /**
    * Type of object for DeathZone
    */
    static const int objectType = 5;
    /**
    * Create a new DeathZone with predefined size and position
    */
	DeathZone(sf::Vector2f size, sf::Vector2f pos);
    /**
    * Create an empty DeathZone. Still sets GameObject appropriate;y
    */
    DeathZone();

    /**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
    std::string toString();

    /**
    * Creates a new object using the string provided. To work with GameWindow, this should match toString.
    */
    std::shared_ptr<GameObject> constructSelf(std::string self);

    /**
    * Creates a blank GameObject. Useful for when you need a copy of the object without knowing what type it is.
    */
    std::shared_ptr<GameObject> makeTemplate();

    /**
    * Return the type of the class. Type should be a static const variable so that it is constant across all versions.
    * type should be unique for each version of GameObject that you use with GameWindow.
    */
    int getObjectType();
};
#endif