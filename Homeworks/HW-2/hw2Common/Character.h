#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

/**
* Class for Character objects, or the shape that is controlled by the player.
* A character has speed, which is the amount of movement the character moves right or left per frame.
* A character also has various states like jumping.
*/
class Character : public sf::RectangleShape {

private:
    /**
    * Movement per frame of the character
    */
    sf::Vector2f speed;

    /**
    * Is the character jumping right now.
    */
    bool jumping = false;

    /**
    * How much does the character fall per second
    */
    float gravity = 0.f;

    /**
    * The ID of the character. Init at -1 for a client who has not yet connected.
    * Server should change this to 0 or a positive value to show it has connected, and return
    * a negative value if it has disconnected
    */
    int id = -1;

public:
    /**
    * Empty constructor
    */
    Character();

    /**
    * Returns the speed of the character.
    * @return the speed of the character
    */
    sf::Vector2f getSpeed();

    /**
    * set the speed of the character
    */
    void setSpeed(float speed);

    /**
    * check if the character is currently jumping
    */
    bool isJumping();

    /**
    * set the state of the character as jumping
    */
    void setJumping(bool jump);

    /*
    * Set the gravity for the character. This should be in the form of pixels/second. Set 0 if the character doesn't have gravity.
    */
    void setGravity(float gravity);

    /**
    * Return the gravity for the character.
    */
    float getGravity();

    /**
    * Set the ID of the character. This should pretty much only be used by the server.
    */
    void setID(int id);

    /**
    * Get the ID of the character.
    */
    int getID();
};
#endif