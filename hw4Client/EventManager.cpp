#include "EventManager.h"

EventManager::EventManager()
{
}

void EventManager::registerEvent(std::list<std::string> list, EventHandler* handler)
{

}

void EventManager::deregister(std::list<std::string> list, EventHandler* handler)
{
}

void EventManager::raise(Event e)
{
	raised_events.push_back(e);
}
