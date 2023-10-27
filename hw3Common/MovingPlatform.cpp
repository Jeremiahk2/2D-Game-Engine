#include "MovingPlatform.h"

    //Bound bound1 is the left/bottom bound. bound 2 is the right/top bound. Type is 0 for vertical, 1 for horizontal.
    MovingPlatform::MovingPlatform(float speed, bool type, float startx, float starty) : Platform()
    {
        startPos = sf::Vector2f(startx, starty);
        setPosition(startPos);
        v_speed.y = speed;
        h_speed.x = speed;
        m_type = type;
        bound1 = 0;
        bound2 = 0;
    }

    void MovingPlatform::setVSpeed(sf::Vector2f speed) {
        std::lock_guard<std::mutex> lock(*getMutex());
        v_speed = speed;
    }

    void MovingPlatform::setHSpeed(sf::Vector2f speed) {
        std::lock_guard<std::mutex> lock(*getMutex());
        h_speed = speed;
    }

    sf::Vector2f MovingPlatform::getSpeed() {
        std::lock_guard<std::mutex> lock(*getMutex());
        if (m_type == 0) {
            return(v_speed);
        }
        return(h_speed);
    }

    bool MovingPlatform::getMovementType() {
        return m_type;
    }

    sf::Vector2f MovingPlatform::getStartPos() {
        return startPos;
    }

    void MovingPlatform::setBounds(int bound1, int bound2) {
        std::lock_guard<std::mutex> lock(*getMutex());
        this->bound1 = bound1;
        this->bound2 = bound2;
    }

    sf::Vector2i MovingPlatform::getBounds() {
        std::lock_guard<std::mutex> lock(*getMutex());
        return sf::Vector2i(bound1, bound2);
    }
    void MovingPlatform::move(float x, float y) {
        float roundedX = std::roundf(100 * x) / 100.f;
        float roundedY = std::roundf(100 * y) / 100.f;
        Platform::move(roundedX, roundedY);
        lastMove = sf::Vector2f(roundedX, roundedY);
    }

    void MovingPlatform::move(sf::Vector2f v) {
        Platform::move(v);
        lastMove = v;
    }

    sf::Vector2f MovingPlatform::getLastMove() {
        std::lock_guard<std::mutex> lock(*getMutex());
        return lastMove;
    }


    
    