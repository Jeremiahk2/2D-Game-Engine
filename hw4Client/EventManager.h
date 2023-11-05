#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "EventHandler.h"
#include <list>
#include <unordered_map>
class EventManager {
public:
	EventManager();

	void registerEvent(std::list<std::string>, EventHandler*);

	void deregister(std::list<std::string>, EventHandler*);

	void raise(Event);

	std::list<Event> raised_events;

	std::unordered_map<std::string, std::list<EventHandler*>> handlers;

private:
};
#endif
