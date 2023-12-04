#include "EventManager.h"

std::map<int, std::multimap<int, Event>> EventManager::raised_events;

EventManager::EventManager()
{
	this->window = NULL;
	this->global = NULL;
}

EventManager::EventManager(GameWindow* window)
{
	this->window = window;
	this->global = NULL;
}

EventManager::EventManager(Timeline* global)
{
	this->window = NULL;
	this->global = global;
}

EventManager::EventManager(GameWindow* window, Timeline* global)
{
	this->window = window;
	this->global = global;
}

GameWindow* EventManager::getWindow()
{
	return window;
}

Timeline* EventManager::getTimeline()
{
	return global;
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

	//If a map at that time already exists, add it to it.
	if (auto search = raised_events.find(e.time); search != raised_events.end()) {
		search->second.insert({ e.order, e });
	}
	//If there is no map at that time, make one.
	else {
		std::multimap<int, Event> newMap;
		newMap.insert({ e.order, e });
		raised_events.insert({e.time, newMap});
	}
}

void EventManager::raiseEventFromScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	const char* value = v8helpers::ToCString(v8::String::Utf8Value(isolate, args[0]->ToString(context).ToLocalChecked()));
	//v8::Local<v8::Value> object = context->Global()->Get(context, args[0]->ToString(context).ToLocalChecked()).ToLocalChecked();
	for (int i = GameObject::game_objects.size() - 1; i >= 0; i--) {
		if (strcmp(value, GameObject::game_objects[i]->guid.c_str()) == 0) {
			raise(*(static_cast<Event*>(GameObject::game_objects[i])));
		}
	}


	//Bad alloc
	//void* real = *object;
	//Event* e = static_cast<Event*>(real);
	//if (e != nullptr) {
	//	raise(*e);
	//}
	//else {
	//	std::cout << "Failed";
	//}

	//Bad alloc
	//v8::Local<v8::Object> real = object->ToObject(context).ToLocalChecked();
	//Event* e = new Event;
	//void* ptr = *real;
	//*e = *(static_cast<Event*>(ptr));
	//raise(*e);

	//Bad alloc
	//Event* e;
	//void* ptr = *object;
	//e = static_cast<Event*>(ptr);
	//raise(*e); 


	//Null pointer
	//v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object);
	//Event* e = new Event;
	//void* ptr = wrap->Value();
	//*e = *(static_cast<Event*>(ptr));
	//raise(*e); 
}
