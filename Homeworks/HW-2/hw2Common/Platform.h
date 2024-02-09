#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

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

    
public:
    /**
    * Constructs a new platform (empty RectangleShape)
    */
    Platform();

    /**
    * Set the passthrough setting on this platform.
    */
    void setPassthrough(bool passthrough);
};

#endif