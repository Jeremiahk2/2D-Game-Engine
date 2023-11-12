#include "Event.h"

Event::Event() : GameObject(false, false, false)
{

}

std::string Event::toString()
{
	
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << time << space << order;

    for (const auto& [key, value] : parameters) {
        stream << space << key;
        if (value.m_Type == Event::variant::TYPE_INT) {
            stream << space << value.m_asInt;
        }
        else if (value.m_Type == Event::variant::TYPE_FLOAT){
            stream << space << value.m_asFloat;
        }
    }

    std::string line;
    getline(stream, line);

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
