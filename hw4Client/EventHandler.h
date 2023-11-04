#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include "Event.h"


class EventHandler {
public:
	virtual void onEvent(Event e) = 0;
};
#endif
