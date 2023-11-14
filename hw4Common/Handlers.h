#ifndef HANDLERS_H
#define HANDLERS_H
#include "EventHandler.h"
#include "MovingPlatform.h"
#include "Platform.h"
#include "DeathZone.h"
#include "SideBound.h"
#include "EventManager.h"
class CollisionHandler : public EventHandler {
public:
	void onEvent(Event e);
};

class MovementHandler : public EventHandler {
public:
	enum DIRECTION {
		LEFT,
		RIGHT,
		JUMP,
		UP
	};
	void onEvent(Event e);
};

//class JumpHandler : public EventHandler {
//private:
//	EventManager* em;
//public:
//	JumpHandler(EventManager* em);
//
//	void onEvent(Event e);
//};

class GravityHandler : public EventHandler {
private:
	EventManager *em;
public:
	GravityHandler(EventManager *em);

	void onEvent(Event e);
};

class SpawnHandler : public EventHandler {
public:

	void onEvent(Event e);
};

class DeathHandler : public EventHandler {
private:
	EventManager* em;
public:
	DeathHandler(EventManager* em);
	void onEvent(Event e);
};

class DisconnectHandler : public EventHandler {
public:
	void onEvent(Event e);
};



#endif
