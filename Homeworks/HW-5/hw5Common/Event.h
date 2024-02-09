#ifndef EVENT_H
#define EVENT_H
#include "GameObject.h"
#include "GameWindow.h"
#include <unordered_map>
#include <zmq.hpp>

class Event : public GameObject {

private:
	v8::Isolate* isolate;
	v8::Global<v8::Context>* context;

	static void setEventGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void getEventGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

	static void setEventType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void getEventType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
	static void setEventTime(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void getEventTime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
	static void setEventOrder(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void getEventOrder(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

public:

	Event();

	~Event();

	static const int objectType = 7;
	struct variant {
		enum Type {
			TYPE_INT,
			TYPE_FLOAT,
			TYPE_BOOLP,
			TYPE_STRING,
			TYPE_GAMEOBJECT,
			TYPE_GAMEWINDOW,
			TYPE_SOCKET
		};
		variant::Type m_Type;
		union {
			int m_asInt;
			float m_asFloat;
			const char* m_asString;
			bool* m_asBoolP;
			zmq::socket_t* m_asSocket;
			GameObject* m_asGameObject;
			GameWindow* m_asGameWindow;
		};
	};

	int order = 0;

	int time = 0;

	std::string type;

	std::unordered_map<std::string, variant> parameters;

	std::string toString() override;

	std::shared_ptr<GameObject> constructSelf(std::string self) override;

	std::shared_ptr<GameObject> makeTemplate() override;

	int getObjectType() override;
	
	static std::unordered_map<std::string, Event*> events;

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
	v8::Local<v8::Object> exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name = "default") override;

	/**
	 * Factor methods for creating new instances of GameObjects.
	 *
	 * This is primarily a demonstration of how you can create new objects
	 * in javascript. NOTE: this is embedded in this GameObject class, but
	 * could (and likely should) be used for other functionality as well
	 * (i.e., Events).
	 */
	 //static GameObject* GameObjectFactory(std::string context_name = "default");
	static void ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args);
};
#endif
