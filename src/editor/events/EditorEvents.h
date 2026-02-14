#pragma once
#include "ID.h"

struct ObjectSelectedEvent{
	int UID;
	bool increament;
};

struct SelectionChangedEvent {
	ObjectID* obj;
};

struct FrameBufferResetEvent {
	void* pass;   // TODO: buffer ID
	void* result; // TODO: buffer ID
};

struct RenderSurfaceResizedEvent {
	int width, height;
};