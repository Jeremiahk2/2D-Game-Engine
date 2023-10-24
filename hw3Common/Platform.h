#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>

/**
* A class for platforms, a type of RectangleShape.
*/
class Platform : public sf::RectangleShape
{
private:
    /**
    * Should the player pass through the Platform when jumping up into it.
    */
    bool passthrough;

    std::mutex *mutex;

    
public:
    /**
    * Constructs a new platform (empty RectangleShape)
    */
    Platform();

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
};

#endif