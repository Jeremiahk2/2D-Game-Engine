#ifndef MP_H
#define MP_H

#include "Platform.h"

/**
* Class for handling a Platform that moves.
* A moving platform has speed, type (vertical or horizontal), and direction (which )
*/
class MovingPlatform: public Platform
{
private:

    /**
    * The type of movement. True for horizontal, false for vertical
    */
    bool m_type;

    /**
    * The speed vectors for vertical and horizontal speed.
    */
    sf::Vector2f speed = sf::Vector2f(0, 0);

    /**
    * the starting position of the platform.
    */
    sf::Vector2f startPos;

    /**
    * Right and left/Top and bottom bounds for movement.
    */
    int bound1;
    int bound2;

    /**
    * The last recorded move that the moving platform made.
    */
    sf::Vector2f lastMove = sf::Vector2f(0, 0); 


public:

    static const int objectType = 3;
    /**
    * Constructs a moving platform with the given values.
    * @param speed the speed of the platform per frame.
    * @param type the type of movement. false for vertical, true for horizontal.
    * @param startx The x coordinate for where the platform should be created.
    * @param starty the y coordinate for where the platform should be created.
    */
    MovingPlatform(float speed, bool type, float startx, float starty);

    /**
    * Empty constructor. Sets GameObject fields appropriately.
    */
    MovingPlatform();

    /**
    * Set the bounds for movement.
    * @param bound1 left/bottom bound
    * @param bound2 right/top bound
    */
    void setBounds(int bound1, int bound2);

    /**
    * Return the type of movement
    * TODO: Rename to isHorizontal()?
    * @return the type of movement. True for horizontal, false for vertical.
    */
    bool getMovementType();

    /**
    * Set the movement type for the platofmr.
    */
    void setMovementType(int movementType);

    /**
    * Return the position that the moving platform started at.
    */
    sf::Vector2f getStartPos();

    /**
    * Return a vector representation of speed.
    */
    sf::Vector2f getSpeedVector();

    /**
    * Return a floating point, direction representation of speed.
    */
    float getSpeedValue();

    /**
    *Set the speed of the moving platform.
    */
    void setSpeed(sf::Vector2f speed);

    /**
    * return bound1 and bound2 as a vector
    */
    sf::Vector2i getBounds();

    /**
    * Move the platform (thread safe)
    */
    void move(float x, float y);

    /**
    * Override of move. Overrided in order to set lastMove
    */
    void move(sf::Vector2f v);

    /**
    * Return the last recorded move 
    */
    sf::Vector2f getLastMove(); 

    /**
    * Get a string representation of the moving platform.
    */
    std::string toString() override;

    /**
    * Create and return a new moving platform using a string.
    */
    std::shared_ptr<GameObject> constructSelf(std::string self) override;

    /**
    * Return MovingPlatform's object type.
    */
    int getObjectType() override;

};

#endif