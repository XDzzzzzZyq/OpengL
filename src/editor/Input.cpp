#include "Input.h"
#include "macros.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Input::InputState Input::input_state = Input::InputState{};
Input::InputState Input::input_state_b = Input::InputState{};

Input::MouseButtons ListenMouseEvent(GLFWwindow* window)
{
	//update
	LOOP(3)
		if (glfwGetMouseButton(window, i) == GLFW_PRESS)
			return Input::MouseButtons(i + 1);

	return Input::MouseButtons::NONE;
}


//https://www.glfw.org/docs/3.3/group__keys.html
//shift -> 340
//ctrl  -> 341
//alt	-> 342
Input::SpecialKeys ListenSpecialKeyEvent(GLFWwindow* window)
{
	Input::SpecialKeys keys = Input::NONE;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		keys = static_cast<Input::SpecialKeys>(keys | Input::SHIFT);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		keys = static_cast<Input::SpecialKeys>(keys | Input::CTRL);
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
		keys = static_cast<Input::SpecialKeys>(keys | Input::ALT);

	return keys;
}

// A -> 1 | Z -> 26
int ListenNormalKeyEvent(GLFWwindow* window)
{
	LOOP(26)
		if (glfwGetKey(window, GLFW_KEY_A + i) == GLFW_PRESS)
			return i + 1;

	for (int i = 0; i < 10 && (26 + i) < NORM_KEY_LEN; ++i)
		if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			return 26 + i + 1;

	return 0;
}

static void ScrollCallback(GLFWwindow* /*window*/, double xoffset, double yoffset)
{
	Input::input_state.mouse.scroll_x = static_cast<float>(xoffset);
	Input::input_state.mouse.scroll_y = static_cast<float>(yoffset);
}

#include "xdz_math.h"
void Input::UpdateState(GLFWwindow* window) const
{
	input_state_b = input_state;
	input_state.mouse.scroll_x = 0;
	input_state.mouse.scroll_y = 0;
	glfwPollEvents();

	/*		Mouse Input 	*/

	input_state.mouse.button = ListenMouseEvent(window);

	double _mouse_x, _mouse_y;
	glfwGetCursorPos(window, &_mouse_x, &_mouse_y);
	input_state.mouse.mouse_x = static_cast<float>(_mouse_x);
	input_state.mouse.mouse_y = static_cast<float>(_mouse_y);

	glfwSetScrollCallback(window, ScrollCallback);

	/*	  KeyBoard Input 	*/

	input_state.key.special = ListenSpecialKeyEvent(window);

	input_state.key.normal = ListenNormalKeyEvent(window);

	/*		Global Randoms  	*/

	input_state.random.random_float1 = xdzm::rand01();
	input_state.random.random_float2 = xdzm::rand01();
	input_state.random.random_float3 = xdzm::rand01();
	input_state.random.random_float4 = xdzm::rand01();

	input_state.viewport.frame_count++;
}

bool Input::IsMouseClicked()
{
	return input_state.mouse.button != MouseButtons::NONE && input_state_b.mouse.button == MouseButtons::NONE;
}

bool Input::IsMousePressed()
{
	return input_state.mouse.button != MouseButtons::NONE;
}

bool Input::IsMouseLeft()
{
	return input_state.mouse.button == MouseButtons::NONE && input_state_b.mouse.button != MouseButtons::NONE;
}

bool Input::IsKeyClicked()
{
	const bool pressed_now = input_state.key.special != NONE || input_state.key.normal != 0;
	const bool pressed_before = input_state_b.key.special != NONE || input_state_b.key.normal != 0;
	return pressed_now && !pressed_before;
}

bool Input::IsKeyPressed()
{
	return input_state.key.special != NONE || input_state.key.normal != 0;
}

bool Input::IsKeyLeft()
{
	const bool pressed_now = input_state.key.special != NONE || input_state.key.normal != 0;
	const bool pressed_before = input_state_b.key.special != NONE || input_state_b.key.normal != 0;
	return !pressed_now && pressed_before;
}

bool Input::IsMouseScrolled()
{
	return input_state.mouse.scroll_x != 0 || input_state.mouse.scroll_y != 0;
}

float Input::GetMousePosX()
{
	return input_state.mouse.mouse_x;
}

float Input::GetMousePosY()
{
	return input_state.mouse.mouse_y;
}

float Input::GetDeltaMouseX()
{
	return input_state.mouse.mouse_x - input_state_b.mouse.mouse_x;
}

float Input::GetDeltaMouseY()
{
	return input_state.mouse.mouse_y - input_state_b.mouse.mouse_y;
}

float Input::GetScrollX()
{
	return input_state.mouse.scroll_x;
}

float Input::GetScrollY()
{
	return input_state.mouse.scroll_y;
}

bool Input::IsSelectedChanged()
{
	return input_state.viewport.is_selected_changed;
}

bool Input::IsGOListChanged()
{
	return input_state.viewport.is_GOlist_changed;
}

bool Input::IsSpriteSelected()
{
	return input_state.viewport.is_sprite_selected;
}

void Input::ResetFrameCount(int count)
{
	input_state.viewport.frame_count = count;
}

int Input::GetFrameCount()
{
	return input_state.viewport.frame_count;
}