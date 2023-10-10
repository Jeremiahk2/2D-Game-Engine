#include "MovingPlatform.h"
//Correct version

    bool m_type;

    sf::Vector2f startPos;

    sf::Vector2f v_speed = sf::Vector2f(0, 0);
    sf::Vector2f h_speed = sf::Vector2f(0, 0);

    int bound1;
    int bound2;

    sf::Vector2f lastMove = sf::Vector2f(0, 0);

    //Bound bound1 is the left/bottom bound. bound 2 is the right/top bound. Type is 0 for vertical, 1 for horizontal.
    MovingPlatform::MovingPlatform(float speed, bool type, float startx, float starty) : Platform()
    {
        startPos = sf::Vector2f(startx, starty);
        setPosition(startPos);
        v_speed.y = speed;
        h_speed.x = speed;
        m_type = type;
    }

    sf::Vector2f MovingPlatform::getSpeed() {
        if (m_type == 0) {
            return(v_speed);
        }
        return(h_speed);
    }

    bool MovingPlatform::getType() {
        return m_type;
    }

    sf::Vector2f MovingPlatform::getStartPos() {
        return startPos;
    }

    void MovingPlatform::setBounds(int bound1, int bound2) {
        this->bound1 = bound1;
        this->bound2 = bound2;
    }

    sf::Vector2i MovingPlatform::getBounds() {
        return sf::Vector2i(bound1, bound2);
    }

    void MovingPlatform::setVSpeed(sf::Vector2f speed) {
        v_speed = speed;
    }

    void MovingPlatform::setHSpeed(sf::Vector2f speed) {
        h_speed = speed;
    }
    void MovingPlatform::move(float x, float y) {
        Platform::move(x, y);
        lastMove = sf::Vector2f(x, y);
    }

    void MovingPlatform::move(sf::Vector2f v) {
        Platform::move(v);
        lastMove = v;
    }

    sf::Vector2f MovingPlatform::getLastMove() {
        return lastMove;
    }


    
    