#pragma once

#include "Event.h"

class EventListener {
public:
	virtual void bind(EventPool& bus) = 0;
};