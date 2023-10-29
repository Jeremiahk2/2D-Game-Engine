#ifndef SIDEBOUND_H
#define SIDEBOUND_H
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "GameObject.h"
#include "GameWindow.h"


class SideBound : public GameObject, public sf::RectangleShape {
private:
	sf::View view;

	GameWindow* window;
public:

	const static int objectType = 6;

	SideBound();

	SideBound(GameWindow *window);

	SideBound(GameWindow *window, sf::View view);

	void setView(sf::View view);

    void onCollision();

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

    /**
    * Return the type of the class. Type should be a static const variable so that it is constant across all versions.
    * type should be unique for each version of GameObject that you use with GameWindow.
    */
    int getObjectType() override;
};
#endif
