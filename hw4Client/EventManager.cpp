#include "EventManager.h"

EventManager::EventManager()
{
}

void EventManager::registerEvent(std::list<std::string> list, EventHandler* handler)
{
	for (std::string i : list) {
		if (handlers.count(i)) {
			handlers[i].push_back(handler);
		}
		else {
			std::list<EventHandler*> newList;
			newList.push_back(handler);
			handlers.insert({ i, newList });
		}
	}
}

void EventManager::deregister(std::list<std::string> list, EventHandler* handler)
{
	for (std::string i : list) {
		if (handlers.count(i)) {
			handlers[i].remove(handler);
			if (handlers[i].empty()) {
				handlers.erase(i);
			}
		}
	}
}

void EventManager::raise(Event e)
{
	raised_events.push_back(e);
}
