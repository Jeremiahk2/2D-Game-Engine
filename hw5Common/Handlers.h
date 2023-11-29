#ifndef HANDLERS_H
#define HANDLERS_H
#include "EventHandler.h"
#include "MovingPlatform.h"
#include "Platform.h"
#include "DeathZone.h"
#include "SideBound.h"
#include "EventManager.h"
#include <zmq.hpp>
class CollisionHandler : public EventHandler {
public:
	void onEvent(Event e) override;
};

class MovementHandler : public EventHandler {
public:
	enum DIRECTION {
		LEFT,
		RIGHT,
		JUMP,
		UP
	};
	void onEvent(Event e) override;
};

class GravityHandler : public EventHandler {
private:
	EventManager *em;
public:
	GravityHandler(EventManager *em);

	void onEvent(Event e) override;
};

class SpawnHandler : public EventHandler {
public:

	void onEvent(Event e) override;
};

class DeathHandler : public EventHandler {
private:
	EventManager* em;
public:
	DeathHandler(EventManager* em);
	void onEvent(Event e) override;
};

class DisconnectHandler : public EventHandler {
public:
	void onEvent(Event e) override;
};

class ClosedHandler : public EventHandler {
private:
	EventManager* em;
public:

	ClosedHandler();
	ClosedHandler(EventManager* em);

	void onEvent(Event e) override;
};

class NetworkHandler : public EventHandler {
private:
	EventManager* em;
public:
	NetworkHandler(EventManager* em);

	void onEvent(Event e) override;
};



#endif
