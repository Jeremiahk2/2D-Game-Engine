#include "Event.h"

Event::Event() : GameObject(false, false, false)
{

}

std::string Event::toString()
{
	
    //std::stringstream stream;
    //char space = ' ';

    //stream << getObjectType();

    //for (const auto& [key, value] : paramaters) {
    //    stream << space <<
    //}
    //    

    //stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y
    //    << space << (int)getFillColor().r << space << (int)getFillColor().g << space << (int)getFillColor().b << space << getMovementType()
    //    << space << getSpeedValue();

    std::string line;
    //getline(stream, line);

    return line;
}

std::shared_ptr<GameObject> Event::constructSelf(std::string self)
{
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> Event::makeTemplate()
{
	return std::shared_ptr<GameObject>(new Event);
}

int Event::getObjectType()
{
	return objectType;
}
