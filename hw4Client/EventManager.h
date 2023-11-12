#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "EventHandler.h"
#include "Timeline.h"
#include <list>
#include <unordered_map>
class EventManager {
public:
	EventManager();

	EventManager(GameWindow* window);

	EventManager(Timeline* global);

	EventManager(GameWindow* window, Timeline* global);

	GameWindow* getWindow();

	Timeline* getTimeline();


	void registerEvent(std::list<std::string>, EventHandler*);

	void deregister(std::list<std::string>, EventHandler*);

	void raise(Event e);

	std::map<int, std::multimap<int, Event>> raised_events;

	std::unordered_map<std::string, std::list<EventHandler*>> handlers;

private:

	GameWindow* window;
	Timeline* global;
};
#endif
