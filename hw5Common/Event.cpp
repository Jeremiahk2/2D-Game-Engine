#include "Event.h"

Event::Event() : GameObject(false, false, false)
{

}

std::string Event::toString()
{
	
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << time << space << order << space << type << space;

    for (const auto& [key, value] : parameters) {
        if (value.m_Type == Event::variant::TYPE_INT) {
            stream << key << space << Event::variant::TYPE_INT << space << value.m_asInt << ".";
        }
        else if (value.m_Type == Event::variant::TYPE_FLOAT){
            stream << key << space << Event::variant::TYPE_FLOAT << space << value.m_asFloat << ".";
        }
        else if (value.m_Type == Event::variant::TYPE_BOOLP) {
            stream << key << space << Event::variant::TYPE_BOOLP << space << *(value.m_asBoolP) << ".";
        }
        else if (value.m_Type == Event::variant::TYPE_STRING) {
            stream << key << space << Event::variant::TYPE_STRING << space << value.m_asString << ".";
        }
    }
    std::string line;
    getline(stream, line);
    return line;
}

std::shared_ptr<GameObject> Event::constructSelf(std::string self)
{
    int pos = 0;
    int newPos = 0;
    Event *e = new Event();
    int objectType;
    int time;
    int order;
    char* type = (char*)malloc(self.size() + 1);

    //Get the time and the order of the event.
    if (sscanf_s(self.data(), "%d %d %d %s%n", &objectType, &time, &order, type, (unsigned int)self.size() + 1, &pos) != 4) {
        throw std::invalid_argument("Failed to read string. Not an event");
    }
    e->time = time;
    e->order = order;
    e->type = type;

    char* current = (char*)malloc(self.size() + 1);
    //Scan through each variant
    while (sscanf_s(self.data() + pos, "%[^.]%n", current, (unsigned int)(self.size() + 1), &newPos) == 1) {

        char* key = (char*)malloc(strlen(current) + 1);
        Event::variant::Type type;
        int innerPos = 0;

        //Get the key and the type of value
        int matches = sscanf_s(current, "%s %d%n", key,(unsigned int)strlen(current) + 1, & type, &innerPos);
        //Throw if key or type is invalid
        if (matches != 2) {
            free(key);
            throw std::invalid_argument("Failed to read string. Type must be the first value.");
        }
        //If it's an int
        if (key && type == Event::variant::TYPE_INT) {
            int value;
            sscanf_s(current + innerPos, "%d", &value);
            Event::variant intVariant;
            intVariant.m_Type = Event::variant::TYPE_INT;
            intVariant.m_asInt = value;
            e->parameters.insert({ std::string(key), intVariant });
        }
        //If it's a float
        else if (key && type == Event::variant::TYPE_FLOAT) {
            float value;
            sscanf_s(current + innerPos, "%f", &value);
            Event::variant floatVariant;
            floatVariant.m_Type = Event::variant::TYPE_FLOAT;
            floatVariant.m_asFloat = value;
            e->parameters.insert({ std::string(key), floatVariant });
        }
        //If it's a boolean
        else if (key && type == Event::variant::TYPE_BOOLP) {
            int value;
            sscanf_s(current + innerPos, "%d", &value);
            bool *boolValue = new bool(value);
            Event::variant boolVariant;
            boolVariant.m_Type = Event::variant::TYPE_BOOLP;
            boolVariant.m_asBoolP = boolValue;
            e->parameters.insert({ std::string(key), boolVariant });
        }
        //If it's a string
        else if (key && type == Event::variant::TYPE_STRING) {
            char* value = (char *)malloc(strlen(current) + 1);
            sscanf_s(current + innerPos, " %[^.]", value, (unsigned int)strlen(current) + 1);
            Event::variant stringVariant;
            stringVariant.m_Type = Event::variant::TYPE_STRING;
            stringVariant.m_asString = value;
            e->parameters.insert({ std::string(key), stringVariant });
        }


        //update position and skip past comma
        pos += newPos + 1;
    }
    free(current);
    std::shared_ptr<GameObject> rtnptr(e);
    return rtnptr;
}

std::shared_ptr<GameObject> Event::makeTemplate()
{
	return std::shared_ptr<GameObject>(new Event);
}

int Event::getObjectType()
{
	return objectType;
}
