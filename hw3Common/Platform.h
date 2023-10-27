#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>
#include "GameObject.h"

/**
* A class for platforms, a type of RectangleShape.
*/
class Platform : public GameObject, public sf::RectangleShape
{
private:
    /**
    * Should the player pass through the Platform when jumping up into it.
    */
    bool passthrough;

    std::mutex innerMutex;

    
public:

    static const int objectType = 2;

    /**
    * Constructs a new platform (empty RectangleShape)
    */
    Platform();

    Platform(bool stationary, bool collidable, bool drawable);

    /**
    * Set the passthrough setting on this platform.
    */
    void setPassthrough(bool passthrough);

    void move(float offsetX, float offsetY);

    void move(const sf::Vector2f offset);

    void setPosition(float x, float y);

    void setPosition(const sf::Vector2f position);

    sf::Vector2f getPosition();

    sf::FloatRect getGlobalBounds();

    std::mutex* getMutex();

    /**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
    std::string toString() override;

    /**
    * Creates a new object using the string provided. To work with GameWindow, this should match toString.
    */
    std::shared_ptr<GameObject> constructSelf(std::string self) override;

    /**
    * Creates a blank GameObject. Useful for when you need a copy of the object without knowing what type it is.
    */
    std::shared_ptr<GameObject> makeTemplate() override;

    /**
    * Return the type of the class. Type should be a static const variable so that it is constant across all versions.
    * type should be unique for each version of GameObject that you use with GameWindow.
    */
    int getObjectType() override;
};

#endif