#ifndef SPAWNPOINT_H
#define SPAWNPOINT_H
#include "GameObject.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class SpawnPoint : public GameObject, public sf::RectangleShape {
private:


public:
    /**
    * object type of SpawnPoints
    */
    const static int objectType = 4;

    /**
    * create a new spawn point at this position
    */
    SpawnPoint(float x, float y);
    /**
    * create a new spawn point at this position
    */
    SpawnPoint(sf::Vector2f pos);
    /**
    * create a new spawn pointat 0,0.
    */
    SpawnPoint();


    /**
    * Return the type of object
    */
    int getObjectType() override;

    /**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
    std::string toString() override;

    /**
    * Creates a new object using the string provided. To work with GameWindow, this should match toString.
    */
    std::shared_ptr<GameObject> constructSelf(std::string self) override;

    /**
    * Creates a blank GameObject. Useful for when you need a copy of the object without knowing what type it is.
    */
    std::shared_ptr<GameObject> makeTemplate() override;


};
#endif