#pragma once
#include "Input.h"

struct MouseClickEvent
{
	Input::SpecialKeys key;
	Input::MouseButtons mouse;
	float x, y;
};

struct MouseLeaveEvent
{
	Input::SpecialKeys key;
	Input::MouseButtons mouse;
	float x, y;
};

struct MouseDragEvent
{
	Input::SpecialKeys key;
	Input::MouseButtons mouse;
	float x, y;
	float delta_x, delta_y;
};

struct MouseScrolledEvent
{
	Input::SpecialKeys key;
	float delta_x, delta_y;
};

struct KeyClickEvent
{
	Input::KeyState key;
};

struct KeyLeaveEvent
{
	Input::KeyState key;
};



