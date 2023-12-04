#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "EventHandler.h"
#include "Timeline.h"
#include "GameObject.h"
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

	static void raise(Event e);

	static std::map<int, std::multimap<int, Event>> raised_events;

	std::unordered_map<std::string, std::list<EventHandler*>> handlers;
    //Scripting stuff

	static void raiseEventFromScript(const v8::FunctionCallbackInfo<v8::Value>& args);

    std::string guid;

    /**
     * This function will make this class instance accessible to scripts in
     * the given context.
     *
     * IMPORTANT: Please read this definition of this function in
     * GameObject.cpp. The helper function I've provided expects certain
     * parameters which you must use in order to take advance of this
     * convinience.
     */
    v8::Local<v8::Object> exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name = "default");

private:

	GameWindow* window;
	Timeline* global;
};
#endif
