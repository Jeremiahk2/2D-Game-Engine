#include "DeathZone.h"

DeathZone::DeathZone(sf::Vector2f size, sf::Vector2f pos) : GameObject(true, true, false), sf::RectangleShape()
{
	setSize(size);
	setPosition(pos);
}

DeathZone::DeathZone() : GameObject(true, true, false), sf::RectangleShape() {}

std::string DeathZone::toString()
{
	std::stringstream stream;
	char space = ' ';

	stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y;
	std::string line;
	std::getline(stream, line);
	return line;
}

std::shared_ptr<GameObject> DeathZone::constructSelf(std::string self)
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

	std::shared_ptr<GameObject> ptr(new DeathZone(sf::Vector2f(x, y), sf::Vector2f(sizeX, sizeY)));
	return ptr;
}

std::shared_ptr<GameObject> DeathZone::makeTemplate()
{
	return std::shared_ptr<GameObject>(new DeathZone);
}

int DeathZone::getObjectType()
{
	return objectType;
}
