#pragma once

#include "Events.h"

class EventListener {
public:
	virtual void bind(EventPool& bus) = 0;
};