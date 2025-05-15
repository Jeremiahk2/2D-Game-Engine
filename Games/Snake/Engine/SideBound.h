#ifndef SIDEBOUND_H
#define SIDEBOUND_H
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "GameObject.h"
#include "GameWindow.h"


class SideBound : public GameObject, public sf::RectangleShape {
private:
    /**
    * The view that is displayed on collision.
    */
	sf::View view;
    /**
    * The window that it is displayed to.
    */
	GameWindow* window;
public:
    /**
    * The type of object 
    */
	const static int objectType = 6;
    /**
    * Empty constructor for sidebound.
    */
	SideBound();
    /**
    * Constructs sidebound using a window pointer
    */
	SideBound(GameWindow *window);
    /**
    * Constructs sidebound using a window pointer and a view
    */
	SideBound(GameWindow *window, sf::View view);
    /**
    * Set the view to something different.
    */
	void setView(sf::View view);
    /**
    * onCollision method. Sets the window's view to the view stored in this object.
    */
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
