#pragma once
#include "Input.h"

struct MouseClickEvent
{
	Input::SpecialKeys key;
	Input::MouseStatus mouse;
	float x, y;
};

struct MouseLeaveEvent
{
	Input::SpecialKeys key;
	Input::MouseStatus mouse;
	float x, y;
};

struct MouseDragEvent
{
	Input::SpecialKeys key;
	Input::MouseStatus mouse;
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
	Input::SpecialKeys first_key;
	Input::SpecialKeys second_key;
	int norm_key;
};

struct KeyLeaveEvent
{
	Input::SpecialKeys first_key;
	Input::SpecialKeys second_key;
	int norm_key;
};



