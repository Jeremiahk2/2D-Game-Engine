#include "MovingPlatform.h"

    //Bound bound1 is the left/bottom bound. bound 2 is the right/top bound. Type is 0 for vertical, 1 for horizontal.
    MovingPlatform::MovingPlatform(float speed, bool type, float startx, float starty) : Platform(false, true, true)
    {
        startPos = sf::Vector2f(startx, starty);
        setPosition(startPos);
        m_type = type;
        if (type) {
            this->speed = sf::Vector2f(speed, 0);
        }
        else {
            this->speed = sf::Vector2f(0, speed);
        }
        bound1 = 0;
        bound2 = 0;
    }

    MovingPlatform::MovingPlatform() : Platform(false, true, true) {
        bound1 = 0;
        bound2 = 0;
        m_type = 0;
    }

    void MovingPlatform::setSpeed(sf::Vector2f speed) {
        std::lock_guard<std::mutex> lock(*getMutex());
        this->speed = speed;
    }

    void MovingPlatform::setMovementType(int movementType)
    {
        this->m_type = movementType;
    }

    sf::Vector2f MovingPlatform::getSpeedVector() {
        std::lock_guard<std::mutex> lock(*getMutex());
        return speed;
    }

    float MovingPlatform::getSpeedValue() {
        if (getMovementType()) {
            return speed.x;
        }
        return speed.y;
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

    std::string MovingPlatform::toString()
    {
        std::stringstream stream;
        char space = ' ';

        stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y
            << space << getFillColor().r << space << getFillColor().g << space << getFillColor().b << space << getMovementType() 
            << space << getSpeedValue();
        std::string line;
        std::getline(stream, line);
        return line;
    }

    std::shared_ptr<GameObject> MovingPlatform::constructSelf(std::string self)
    {
        MovingPlatform* c = new MovingPlatform;
        int type;
        float x;
        float y;
        float sizeX;
        float sizeY;
        int r;
        int g;
        int b;
        int movementType;
        float speed;
        int matches = sscanf_s(self.data(), "%d %f %f %f %f %d %d %d %d %f", &type, &x, &y, &sizeX, &sizeY, &r, &g, &b, &movementType, &speed);
        if (matches != 10 || type != getObjectType()) {
            throw std::invalid_argument("Type was not correct for character or string was formatted wrong.");
        }

        c->setPosition(x, y);
        sf::Vector2f size(sizeX, sizeY);
        c->setSize(size);
        sf::Color color(r, g, b);
        c->setFillColor(color);
        c->setMovementType(movementType);
        if (movementType) {
            c->setSpeed(sf::Vector2f(speed, 0));
        }
        else {
            c->setSpeed(sf::Vector2f(0, speed));
        }

        GameObject* go = (GameObject*)c;
        std::shared_ptr<GameObject> ptr(go);
        return ptr;
    }


    
    