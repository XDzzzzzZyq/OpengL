#include "Events.h"
#include "Input.h"

#include "events/KeyMouseEvents.h"

void EventPool::EmitGlobalEvent()
{
	const Input::InputState state = Input::input_state;
	if (Input::IsMouseClicked()) {
		emit(MouseClickEvent{ 
			state.key.FirstKey, 
			state.mouse.button, 
			state.mouse.mouse_x,
			state.mouse.mouse_y});
	}

	if (Input::IsMouseLeft()) {
		emit(MouseLeaveEvent{
			state.key.FirstKey,
			state.mouse.button,
			state.mouse.mouse_x,
			state.mouse.mouse_y });
	}

	if (Input::IsKeyClicked()) {
		emit(KeyClickEvent{
			state.key.FirstKey,
			state.key.SecondKey,
			state.key.NormKey });
	}

	if (Input::IsKeyLeft()) {
		emit(KeyLeaveEvent{
			state.key.FirstKey,
			state.key.SecondKey,
			state.key.NormKey });
	}
}
