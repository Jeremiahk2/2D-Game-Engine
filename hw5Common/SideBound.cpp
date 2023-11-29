#include "SideBound.h"

SideBound::SideBound() : GameObject(true, true, false), sf::RectangleShape()
{
	this->window = nullptr;
}

SideBound::SideBound(GameWindow *window) : GameObject(true, true, false), sf::RectangleShape()
{
	this->window = window;
}

SideBound::SideBound(GameWindow *window, sf::View view) : GameObject(true, true, false), sf::RectangleShape()
{
	this->window = window;
	this->view = view;
}

void SideBound::setView(sf::View view)
{
	this->view = view;
}

void SideBound::onCollision()
{
	window->setView(view);
}

std::string SideBound::toString()
{
	std::stringstream stream;
	char space = ' ';

	stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y
		<< space << view.getCenter().x << space << view.getCenter().y;
	std::string line;
	std::getline(stream, line);
	return line;
}

std::shared_ptr<GameObject> SideBound::constructSelf(std::string self)
{
	int type = 0;
	float x = 0;
	float y = 0;
	float sizeX = 0;
	float sizeY = 0;

	int matches = sscanf_s(self.data(), "%d %f %f %f %f", &type, &x, &y, &sizeX, &sizeY);
	if (matches != 5 || type != getObjectType()) {
		throw std::invalid_argument("Type was not correct for Spawn point or string was formatted wrong.");
	}

	SideBound* ptr = new SideBound();
	ptr->setPosition(x, y);
	ptr->setSize(sf::Vector2f(sizeX, sizeY));
	std::shared_ptr<GameObject> rtnptr(ptr);
	return rtnptr;
}

std::shared_ptr<GameObject> SideBound::makeTemplate()
{
	return std::shared_ptr<GameObject>(new SideBound);
}

int SideBound::getObjectType()
{
	return objectType;
}


