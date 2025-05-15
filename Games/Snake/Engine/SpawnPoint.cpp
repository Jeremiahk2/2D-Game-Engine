#include "SpawnPoint.h"

SpawnPoint::SpawnPoint(float x, float y) : GameObject(true, false, false), sf::RectangleShape()
{
	setPosition(x, y);
}

SpawnPoint::SpawnPoint(sf::Vector2f pos) : GameObject(true, false, false), sf::RectangleShape()
{
	setPosition(pos);
}

SpawnPoint::SpawnPoint() : GameObject(true, false, false), sf::RectangleShape()
{
	//Nothing else needed. RectangleShape does it for us.
}

int SpawnPoint::getObjectType()
{
	return objectType;
}

std::string SpawnPoint::toString()
{
	std::stringstream stream;
	char space = ' ';

	stream << getObjectType() << space << getPosition().x << space << getPosition().y;
	std::string line;
	std::getline(stream, line);
	return line;
}

std::shared_ptr<GameObject> SpawnPoint::constructSelf(std::string self)
{
    int type = 0;
    float x = 0;
    float y = 0;

    int matches = sscanf_s(self.data(), "%d %f %f", &type, &x, &y);
    if (matches != 3 || type != getObjectType()) {
        throw std::invalid_argument("Type was not correct for Spawn point or string was formatted wrong.");
    }

    std::shared_ptr<GameObject> ptr(new SpawnPoint(x, y));
    return ptr;
}

std::shared_ptr<GameObject> SpawnPoint::makeTemplate()
{
	return std::shared_ptr<GameObject>(new SpawnPoint(0, 0));
}


