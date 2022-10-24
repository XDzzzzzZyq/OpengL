#include "EventListener.h"

KeyMouseEvent EventListener::EVT_STATUS;
GLFWwindow* EventListener::evt_window = (GLFWwindow*)nullptr;

bool EventListener::is_key_pressed;
bool EventListener::is_mouse_pressed;
bool EventListener::is_key_pressed_b;
bool EventListener::is_mouse_pressed_b;
double EventListener::mouse_x;
double EventListener::mouse_y;
double EventListener::mouse_b_x;
double EventListener::mouse_b_y;

std::vector<int> EventListener::EVT_NK_LIST = {};

void EventListener::PushNormKey(int _ID)
{
	for (auto& i : EVT_NK_LIST)
		if (i == _ID)
			return;

	EVT_NK_LIST.push_back(_ID);
}

void EventListener::PushNormKey(char _name)
{
	for (auto& i : EVT_NK_LIST)
		if (i == (int)_name)
			return;

	EVT_NK_LIST.push_back((int)_name);
}

bool EventListener::is_spirit_selected = false;
bool EventListener::is_GOlist_changed = true;
bool EventListener::is_selected_changed = true;
bool EventListener::is_outliner_selected = false;

std::function<void* (int)> EventListener::GetActiveShader = [&](int) { return (void*)nullptr;};

int EventListener::active_GO_ID;
int EventListener::pre_act_go_ID;

std::vector<int> EventListener::selec_list;
std::vector<int> EventListener::parent_index_list;
OutlineData EventListener::outline_list;

ShaderLib* EventListener::active_shader = (ShaderLib*)nullptr;

ImVec2 EventListener::window_pos = ImVec2(0, 0);
ImVec2 EventListener::viewport_offset = ImVec2(0, 0);
bool EventListener::is_in_viewport = false;

EventListener::EventListener()
{
}

EventListener::EventListener(GLFWwindow* window)
{
	evt_window = window;
}

EventListener::~EventListener()
{

}

int EventListener::ListenMouseEvent(GLFWwindow* window) const
{
	is_mouse_pressed_b = is_mouse_pressed;

	//update
	LOOP(3)
		if (glfwGetMouseButton(window, i) == GLFW_PRESS) {
			is_mouse_pressed = true;
			return i + 1;
		}


	is_mouse_pressed = false;
	return 0;
}


//https://www.glfw.org/docs/3.3/group__keys.html
//shift->340
//ctrl
//alt
int EventListener::ListenSpecialKeyEvent(GLFWwindow* window, int ignor) const
{
	LOOP(3)
		if (glfwGetKey(window, 340 + i) == GLFW_PRESS)
			if (ignor != i + 1) {
				return i + 1;
			}

	is_key_pressed = false;
	return 0;//no key is pressed
}

// A -> 1 | Z -> 26
int EventListener::ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist) const
{
	if (IDlist.size() == 0)
		return 0;
	LOOP(IDlist.size())
		if (glfwGetKey(window, IDlist[i] + 64) == GLFW_PRESS)
			return IDlist[i];
	return 0;
}

float EventListener::scroll_dir = 80;
bool EventListener::is_scr_changed = false;

void EventListener::UpdateEvent(GLFWwindow* window) const
{
	mouse_b_x = mouse_x;
	mouse_b_y = mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	glfwSetScrollCallback(window, EventListener::scrollCall);
	//spe_key update
	if (ListenSpecialKeyEvent(window, 0) == 0)
	{
		EVT_STATUS.FirstKey = 0;
		EVT_STATUS.SecondKey = 0;
	}
	else {
		EVT_STATUS.FirstKey = ListenSpecialKeyEvent(window, 0);
		EVT_STATUS.SecondKey = ListenSpecialKeyEvent(window, EVT_STATUS.FirstKey);
	}

	if (EVT_NK_LIST.size())
		EVT_STATUS.NormKey = ListenNormalKeyEvent(window, EVT_NK_LIST);

	if (is_scr_changed) {
		EVT_STATUS.Scr = scroll_dir;
	}
	else {
		EVT_STATUS.Scr = 0;
	}

	EVT_STATUS.Mouse = ListenMouseEvent(window);

}

const KeyMouseEvent EventListener::GenIntEvent(int k1, int k2, int k3, int m, int scr)
{
	KeyMouseEvent result;
	result.FirstKey = k1;
	result.SecondKey = k2;
	result.NormKey = k3;
	result.Mouse = m;
	result.Scr = scr;
	result.is_update = false;
	result.is_pressed = false;

	return result;
}

void EventListener::EventActivate()
{
	if (EVT_STATUS.GenStateData() != 0)
		if (EventList.find(EVT_STATUS) != EventList.end())
			EventList[EVT_STATUS]();
}

void EventListener::Reset()
{
	is_GOlist_changed = false;
	is_scr_changed = false;
	is_selected_changed = false;
}

std::vector<std::string> EventListener::EVT_AVAIL_KEYS = { "shift", "ctrl", "alt" };

const KeyMouseEvent EventListener::ParseStrEvent(const std::string& _shortcut)
{
	KeyMouseEvent result;
	std::istringstream str(_shortcut);
	std::string word;

	str >> word;
	LOOP(EVT_AVAIL_KEYS.size()) {
		if (word == EVT_AVAIL_KEYS[i]) {
			result.FirstKey = i+1;
			str >> word;	// "+"
			str >> word;
			LOOP(EVT_AVAIL_KEYS.size()) {
				if (word == EVT_AVAIL_KEYS[i]) {
					result.SecondKey = i+1;
					goto parse_next_norm;
				}
			}
			goto parse_norm;
		}
	}
	goto parse_norm;
parse_next_norm:
	str >> word;	// "+"
	str >> word;
parse_norm:
	result.NormKey = (int)word.data()[0] - 64;
	PushNormKey(result.NormKey);

	return result;
}

int KeyMouseEvent::GenStateData() const
{
	int data = 0;
	data += (FirstKey) * (SecondKey + 2) * 4;
	data += NormKey * 1;
	data += Mouse * 1;
	data += Scr * 3;

	return data;
}
