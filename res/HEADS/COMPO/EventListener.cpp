#include "EventListener.h"

EventListener::EventListener()
{
}

EventListener::~EventListener()
{

}

int EventListener::ListenMouseEvent(GLFWwindow* window) const
{
	LOOP(3) {
		if (glfwGetMouseButton(window, i) == GLFW_PRESS)
		{
			return i;
		}

	}
	return 0;
}


//https://www.glfw.org/docs/3.3/group__keys.html
int EventListener::ListenSpecialKeyEvent(GLFWwindow* window, int ignor) const
{
	LOOP(3) {
		if (glfwGetKey(window, 340 + i) == GLFW_PRESS)
		{
			if (ignor != i + 1)
				return i + 1;
		}

	}
	return 0;//no key is pressed
}

int EventListener::ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist) const
{
	//1~0
	if (IDlist.size()==0)
		return 0;
	LOOP(IDlist.size()) {
		if (glfwGetKey(window, IDlist[i]) == GLFW_PRESS)
		{

			return IDlist[i];
		}
	}
	return 0;
}

void EventListener::UpdateEvent(GLFWwindow* window, const std::vector<int>& IDlist) const
{
	mouse_b_x = mouse_x;
	mouse_b_y = mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);

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

	if(!(bool)IDlist.size())
		evt_KM.Norm_key = ListenNormalKeyEvent(window,IDlist);

	evt_KM.mouse = ListenMouseEvent(window);

	//DEBUG(evt_KM.GenStateData())
}

KeyMouseEvent EventListener::GenIntEvent(int k1, int k2, int k3, int m)
{
	KeyMouseEvent result;
	result.FirstKey = k1;
	result.SecondKey = k2;
	result.Norm_key = k3;
	result.mouse = m;
	result.is_update = false;
	result.is_pressed = false;

	return result;
}

void EventListener::EventActivate(GLFWwindow* window)
{
	UpdateEvent(window, evt_IDlist);
	if (evt_KM.GenStateData() != 0)
		if (EventList.find(evt_KM) != EventList.end())
			EventList[evt_KM]();
}

int KeyMouseEvent::GenStateData() const
{
	int data = 0;
	data += (FirstKey) * (SecondKey + 2) * 4;
	data += Norm_key * 1;
	data += mouse * 1;

	return data;
}
