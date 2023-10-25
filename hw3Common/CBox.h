#ifndef CBOX_H
#define CBOX_H
#include "GameObject.h"

/**
A CBox is a way to link collision boxes (Rects) with their Platforms. 
If a collision is detected, it can be linked to the platform that caused it.
It also contains information about the type of platform so that it is easier to access
*/
class CBox {

private:

    /**
    This is the object that this CBox is attached to.
    */
    GameObject *object;

public:
    /**
    Constructs an empty CBOX.
    */
    CBox();

    /**
    Construct a CBox based on Platforms specifically.
    @param platform a pointer to the platform so that Boudns can be acquired
    */
    CBox(GameObject *object);

    /**
    Return if the object is static or not.
    @return isMoving
    */
    bool isStatic();

    /**
    return the bounding box (as a float rect) for the platform.
    @return the FloatRect attached to this platform (the collision box)
    */
    sf::FloatRect getCBox();

    /**
    Return the platform attached to the CBox
    @return the platform attached to the box
    */
    GameObject *getObject();
};
#endif