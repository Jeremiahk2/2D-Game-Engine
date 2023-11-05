#ifndef HANDLERS_H
#define HANDLERS_H
#include "EventHandler.h"
#include "MovingPlatform.h"
#include "Platform.h"
#include "DeathZone.h"
#include "SideBound.h"
class CollisionHandler : public EventHandler {
public:
	void onEvent(Event e);
};

class MovementHandler : public EventHandler {
	void onEvent(Event e);
};

class GravityHandler : public EventHandler {
	void onEvent(Event e);
};

class DisconnectHandler : public EventHandler {
	void onEvent(Event e);
};



#endif
