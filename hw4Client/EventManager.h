#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "EventHandler.h"
#include <list>
class EventManager {
public:
	EventManager();

	void registerEvent(std::list<std::string>, EventHandler*);

	void deregister(std::list<std::string>, EventHandler*);

	void raise(Event);

private:
	std::map<std::string, std::list<EventHandler*>> handlers;

	std::list<Event> raised_events;
};
#endif
