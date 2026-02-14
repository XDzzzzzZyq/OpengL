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
	void* buffer_obj; // TODO: buffer ID
};

struct RenderSurfaceResizedEvent {
	int width, height;
};