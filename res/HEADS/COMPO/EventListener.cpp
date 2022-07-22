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

	evt_KM.Norm_key = ListenNormalKeyEvent(window,IDlist);

	evt_KM.mouse = ListenMouseEvent(window);
}

KeyMouseEvent EventListener::GenIntEvent(int k1, int k2, int k3, int m, int upd, int prss)
{
	KeyMouseEvent result;
	result.FirstKey = k1;
	result.SecondKey = k2;
	result.Norm_key = k3;
	result.mouse = m;
	result.is_update = (bool)upd;
	result.is_pressed = (bool)prss;

	return result;
}

