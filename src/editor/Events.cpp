#include "Events.h"
#include "Input.h"

#include "events/KeyMouseEvents.h"

void EventPool::EmitGlobalEvent()
{
	const Input::InputState state = Input::GetInputState();
	if (Input::IsMouseClicked()) {
		emit(MouseClickEvent{ 
			state.key.special,
			state.mouse.button, 
			state.mouse.mouse_x,
			state.mouse.mouse_y});
	}

	if (Input::IsMouseLeft()) {
		emit(MouseLeaveEvent{
			state.key.special,
			state.mouse.button,
			state.mouse.mouse_x,
			state.mouse.mouse_y });
	}

	if (Input::IsMouseScrolled()) {
		emit(MouseScrolledEvent{
			state.key.special,
			state.mouse.scroll_x,
			state.mouse.scroll_y });
	}

	if (Input::IsKeyClicked()) {
		emit(KeyClickEvent{ state.key });
	}

	if (Input::IsKeyLeft()) {
		emit(KeyLeaveEvent{ state.key });
	}
}
