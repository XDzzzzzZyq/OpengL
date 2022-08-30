#include "EventListener.h"

KeyMouseEvent EventListener::evt_KM;

bool EventListener::is_key_pressed;
bool EventListener::is_mouse_pressed;
bool EventListener::is_key_pressed_b;
bool EventListener::is_mouse_pressed_b;
double EventListener::mouse_x;
double EventListener::mouse_y;
double EventListener::mouse_b_x;
double EventListener::mouse_b_y;
bool EventListener::is_spirit_selected = false;
bool EventListener::is_GOlist_changed = true;
bool EventListener::is_selected_changed = true;
bool EventListener::is_outliner_selected = false;

int EventListener::active_GO_ID;
int EventListener::pre_act_go_ID;

std::vector<int> EventListener::selec_list;
std::vector<int> EventListener::parent_index_list;
OutlineData EventListener::outline_list;

EventListener::EventListener()
{
}

EventListener::EventListener(GLFWwindow* window)
	:window(window)
{
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

int EventListener::ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist) const
{
	//1~0
	if (IDlist.size()==0)
		return 0;
	LOOP(IDlist.size())
		if (glfwGetKey(window, IDlist[i]) == GLFW_PRESS)
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
		evt_KM.FirstKey = 0;
		evt_KM.SecondKey = 0;
	}
	else {
		evt_KM.FirstKey = ListenSpecialKeyEvent(window, 0);
		evt_KM.SecondKey = ListenSpecialKeyEvent(window, evt_KM.FirstKey);
	}

	if(!(bool)evt_IDlist.size())
		evt_KM.Norm_key = ListenNormalKeyEvent(window,evt_IDlist);

	if (is_scr_changed) {
		evt_KM.scr = scroll_dir;
	}
	else {
		evt_KM.scr = 0;
	}

	evt_KM.mouse = ListenMouseEvent(window);
}

KeyMouseEvent EventListener::GenIntEvent(int k1, int k2, int k3, int m, int scr)
{
	KeyMouseEvent result;
	result.FirstKey = k1;
	result.SecondKey = k2;
	result.Norm_key = k3;
	result.mouse = m;
	result.scr = scr;
	result.is_update = false;
	result.is_pressed = false;

	return result;
}

void EventListener::EventActivate()
{
	if (evt_KM.GenStateData() != 0)
		if (EventList.find(evt_KM) != EventList.end())
			EventList[evt_KM]();
}

void EventListener::Reset()
{
	is_selected_changed = false;
	is_GOlist_changed = false;
	is_scr_changed = false;
}

int KeyMouseEvent::GenStateData() const
{
	int data = 0;
	data += (FirstKey) * (SecondKey + 2) * 4;
	data += Norm_key * 1;
	data += mouse * 1;
	data += scr * 3;

	return data;
}
