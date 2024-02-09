#ifndef MP_H
#define MP_H

#include "Platform.h"
#include <mutex>

/**
* Class for handling a Platform that moves.
* A moving platform has speed, type (vertical or horizontal), and direction (which )
*/
class MovingPlatform: public Platform
{
private:

    /**
    * The type of movement.
    */
    bool m_type;

    /**
    * The speed vectors for vertical and horizontal speed.
    */
    sf::Vector2f v_speed = sf::Vector2f(0, 0);
    sf::Vector2f h_speed = sf::Vector2f(0, 0);

    /**
    * Right and left/Top and bottom bounds for movement.
    */
    int bound1;
    int bound2;

    /**
    * The last recorded move that the moving platform made.
    * TODO: Make thread safe. Add mutex to move?
    */
    sf::Vector2f lastMove = sf::Vector2f(0, 0);


public:
    /**
    * Constructs a moving platform with the given values.
    * @param speed the speed of the platform per frame.
    * @param type the type of movement. false for vertical, true for horizontal.
    * @param startx The x coordinate for where the platform should be created.
    * @param starty the y coordinate for where the platform should be created.
    */
    MovingPlatform(double speed, bool type, double startx, double starty);

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
    bool getType();

    /**
    * Return the position that the moving platform started at.
    */
    sf::Vector2f getStartPos();

    /**
    * Return the current speed (As pixels/second)
    */
    sf::Vector2f getSpeed();

    /**
    * Set the speed of the vertical movement
    */
    void setVSpeed(sf::Vector2f speed);

    /**
    * TODO: Combine these No need to split them up anymore.
    * Set the speed of the horizontal movement
    */
    void setHSpeed(sf::Vector2f speed);

    /**
    * return bound1 and bound2 as a vector
    */
    sf::Vector2i getBounds();

    /**
    * Override of move. Overrided in order to set lastMove
    */
    void move(double x, double y);

    /**
    * Override of move. Overrided in order to set lastMove
    */
    void move(sf::Vector2f v);

    /**
    * Return the last recorded move 
    * TODO: Make thread-safe. Lock-guard?
    */
    sf::Vector2f getLastMove(); 


};

#endif