#include "Input.h"
#include "macros.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

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

static std::string trim(const std::string& s) {
	size_t start = s.find_first_not_of(" \t");
	size_t end = s.find_last_not_of(" \t");
	if (start == std::string::npos) return "";
	return s.substr(start, end - start + 1);
}

Input::KeyState Input::ParseKeyState(const std::string& hotkey)
{
	Input::KeyState state;
	std::stringstream ss(hotkey);
	std::string token;
	bool keyFound = false;

	while (std::getline(ss, token, '+')) {
		token = trim(token);

		if (token == "ctrl") {
			state.special = Input::SpecialKeys(state.special | Input::CTRL);
		}
		else if (token == "shift") {
			state.special = Input::SpecialKeys(state.special | Input::SHIFT);
		}
		else if (token == "alt") {
			state.special = Input::SpecialKeys(state.special | Input::ALT);
		}
		else {
			state.normal = NormalKeyFromChar(token[0]);
		}
	}
	return state;
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

bool Input::IsMousePressed(MouseButtons button)
{
	return input_state.mouse.button == button;
}

bool Input::IsMouseLeft()
{
	return input_state.mouse.button == MouseButtons::NONE && input_state_b.mouse.button != MouseButtons::NONE;
}

bool Input::IsKeyClicked()
{
	const bool special_clicked = input_state.key.special != NONE && input_state_b.key.special == NONE;
	const bool normal_clicked = input_state.key.normal != 0 && input_state_b.key.normal == 0;
	return special_clicked || normal_clicked;
}

bool Input::IsKeyPressed()
{
	return input_state.key.special != NONE || input_state.key.normal != 0;
}

bool Input::IsKeyPressed(SpecialKeys spe_key)
{
	if (spe_key == NONE)
		return input_state.key.special == NONE;
	return (input_state.key.special & spe_key) == spe_key;
}

bool Input::IsKeyLeft()
{
	const bool special_left = input_state.key.special == NONE && input_state_b.key.special != NONE;
	const bool normal_left = input_state.key.normal == 0 && input_state_b.key.normal != 0;
	return special_left || normal_left;
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