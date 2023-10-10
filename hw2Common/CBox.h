#ifndef CBOX_H
#define CBOX_H
//Correct version

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include "MovingPlatform.h"

/**
A CBox is a way to link collision boxes (Rects) with their Platforms. 
If a collision is detected, it can be linked to the platform that caused it.
It also contains information about the type of platform so that it is easier to access
*/
class CBox {

private:

    /**
    Whether or not the object that the CBox is attached to is movable.
    */
    bool movable = false;

    /**
    This is the platform that this CBox is attached to.
    */
    Platform *platform;

public:
    /**
    Constructs an empty CBOX.
    */
    CBox();

    /**
    Construct a CBox based on Platforms specifically.
    @param movable is the Platform a MovingPlatform (or does it change position ever)
    @param platform a pointer to the platform so that Boudns can be acquired
    */
    CBox(bool movable, Platform *platform);

    /**
    Return if the platform is moving.
    @return isMoving
    */
    bool isMoving();

    /**
    return the bounding box (as a float rect) for the platform.
    @return the FloatRect attached to this platform (the collision box)
    */
    sf::FloatRect getCBox();

    /**
    Return the platform attached to the CBox
    @return the platform attached to the box
    */
    Platform *getPlatform();
};
#endif