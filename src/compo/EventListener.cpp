#include "EventListener.h"
#include "macros.h"

EventListener::KeyMouseEvent EventListener::EVT_STATUS;
GLFWwindow* EventListener::evt_window = (GLFWwindow*)nullptr;

int EventListener::frame_count = 0;

float EventListener::random_float1;
float EventListener::random_float2;
float EventListener::random_float3;
float EventListener::random_float4;

bool EventListener::is_key_pressed;
bool EventListener::is_mouse_pressed;
bool EventListener::is_key_pressed_b;
bool EventListener::is_mouse_pressed_b;
bool EventListener::is_key_changed;

float EventListener::mouse_x;
float EventListener::mouse_y;
float EventListener::mouse_b_x;
float EventListener::mouse_b_y;

std::vector<int> EventListener::EVT_NK_LIST = {};

void EventListener::PushNormKey(int _ID)
{
	if(std::find(EVT_NK_LIST.begin(), EVT_NK_LIST.end(), _ID) != EVT_NK_LIST.end())
			return;

	EVT_NK_LIST.push_back(_ID);
}

void EventListener::PushNormKey(char _name)
{
	int id = (int)_name - 64;

	if (std::find(EVT_NK_LIST.begin(), EVT_NK_LIST.end(), id) != EVT_NK_LIST.end())
		return;

	EVT_NK_LIST.push_back(id);
}

bool EventListener::is_sprite_selected = false;
bool EventListener::is_GOlist_changed = true;
bool EventListener::is_selected_changed = true;
bool EventListener::is_outliner_selected = false;

std::function<GameObject* (int)> EventListener::GetActiveObject = NULL;
std::function<GameObject* (void)> EventListener::GetActiveCamera = NULL;

int EventListener::active_GO_ID;

std::vector<int> EventListener::selec_list;
std::vector<int> EventListener::parent_index_list;
OutlineData EventListener::outline_list;

GameObject* EventListener::active_object = (GameObject*)nullptr;

glm::vec2 EventListener::window_pos = glm::vec2(0);
glm::vec2 EventListener::viewport_offset = glm::vec2(0);
bool EventListener::is_in_viewport = false;
EventListener::ViewPortStatus EventListener::viewport_status = EventListener::ViewPortStatus::None;

void EventListener::ReportGuizmoStatus(bool hover, bool click)
{
	if (hover)
		EventListener::viewport_status = EventListener::ViewPortStatus::OnHover;
	if (click)
		EventListener::viewport_status = EventListener::ViewPortStatus::OnClick;
}

EventListener::EventListener()
{
}

EventListener::EventListener(GLFWwindow* window)
{
	evt_window = window;
}

void EventListener::SetWindow(GLFWwindow* window)
{
	evt_window = window;
}

EventListener::~EventListener()
{

}

EventListener::MouseStatus EventListener::ListenMouseEvent(GLFWwindow* window)
{
	//update
	LOOP(3)
		if (glfwGetMouseButton(window, i) == GLFW_PRESS) 
			return MouseStatus(i + 1);

	return MouseStatus::NONE;
}


//https://www.glfw.org/docs/3.3/group__keys.html
//shift -> 340 -> 1
//ctrl  -> 341 -> 2
//alt	-> 342 -> 3
EventListener::SpecialKeys EventListener::ListenSpecialKeyEvent(GLFWwindow* window, SpecialKeys ignor)
{
	LOOP(3)
		if (glfwGetKey(window, 340 + i) == GLFW_PRESS)
			if (ignor != SpecialKeys(i + 1))
				return SpecialKeys(i + 1);

	return SpecialKeys::NONE;//no key is pressed
}

// A -> 1 | Z -> 26
int EventListener::ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist)
{
	if (IDlist.size() == 0)
		return 0;

	LOOP(IDlist.size())
		if (glfwGetKey(window, IDlist[i] + 64) == GLFW_PRESS)
			return IDlist[i];

	return 0;
}

float EventListener::scroll_dir = 0;
bool EventListener::is_scr_changed = false;

#include "xdz_math.h"
void EventListener::UpdateEvent(GLFWwindow* window) const
{
	KeyMouseEvent event_b = EVT_STATUS;
	/*		Mouse Input 	*/	

	mouse_b_x = mouse_x;
	mouse_b_y = mouse_y;
	is_mouse_pressed_b = is_mouse_pressed;

	double _mouse_x, _mouse_y;
	glfwGetCursorPos(window, &_mouse_x, &_mouse_y);
	glfwSetScrollCallback(window, EventListener::scrollCall);

	mouse_x = static_cast<float>(_mouse_x);
	mouse_y = static_cast<float>(_mouse_y);

	/*	  KeyBoard Input 	*/

	is_key_pressed_b = is_key_pressed;

	EVT_STATUS.FirstKey = ListenSpecialKeyEvent(window, SpecialKeys::NONE);
	EVT_STATUS.SecondKey = ListenSpecialKeyEvent(window, EVT_STATUS.FirstKey);

	if (!EVT_NK_LIST.empty())
		EVT_STATUS.NormKey = ListenNormalKeyEvent(window, EVT_NK_LIST);

	EVT_STATUS.Scr = ScrollDir(scroll_dir);

	EVT_STATUS.Mouse = ListenMouseEvent(window);

	/*		Global Randoms  	*/

	if (EventListener::frame_count != 1) {
		EventListener::random_float1 = xdzm::rand01();
		EventListener::random_float2 = xdzm::rand01();
		EventListener::random_float3 = xdzm::rand01();
		EventListener::random_float4 = xdzm::rand01();
	}

	EventListener::is_key_changed = (event_b.FirstKey != EVT_STATUS.FirstKey) || (event_b.SecondKey != EVT_STATUS.SecondKey) || (event_b.NormKey != EVT_STATUS.NormKey);
	EventListener::is_scr_changed = event_b.Scr != EVT_STATUS.Scr;
	EventListener::is_mouse_pressed = EVT_STATUS.Mouse != MouseStatus::NONE;
	EventListener::is_key_pressed = (int)EVT_STATUS.FirstKey + (int)EVT_STATUS.SecondKey + (int)EVT_STATUS.NormKey != 0;

	
	EventListener::frame_count++;
}

EventListener::KeyMouseEvent EventListener::GenIntEvent(int k1, int k2, int k3, int m, int scr)
{
	return EventListener::GenIntEvent(
		SpecialKeys(k1), 
		SpecialKeys(k2), 
		k3,
		MouseStatus(m),
		ScrollDir(scr)
	);
}

EventListener::KeyMouseEvent EventListener::GenIntEvent(SpecialKeys k1, SpecialKeys k2, int k3, MouseStatus m, ScrollDir scr)
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
	is_selected_changed = false;
	scroll_dir = 0;
}

std::vector<std::string> EventListener::EVT_AVAIL_KEYS = { "shift", "ctrl", "alt" };

#include <sstream>
EventListener::KeyMouseEvent EventListener::ParseShortCut(const std::string& _shortcut)
{
	KeyMouseEvent result;
	std::istringstream str(_shortcut);
	std::string word;

	str >> word;
	LOOP(EVT_AVAIL_KEYS.size()) {
		if (word == EVT_AVAIL_KEYS[i]) {
			result.FirstKey = SpecialKeys(i+1);
			str >> word;	// "+"
			str >> word;
			LOOP_N(EVT_AVAIL_KEYS.size(), j) {
				if (word == EVT_AVAIL_KEYS[j]) {
					result.SecondKey = SpecialKeys(j+1);
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

std::unordered_map<std::string, std::unordered_set<std::string>> EventListener::evt_RigisterEvents = {};

void EventListener::REFLRigisterEvent(const std::string& _class_event)
{
	const std::string class_name = _class_event.substr(0, _class_event.find(":"));
	const std::string event_name = _class_event.substr(_class_event.find_last_of(":") + 1, _class_event.size() - 1);

	evt_RigisterEvents[class_name].insert(event_name);
}

void EventListener::ShowEvents()
{
#ifdef _DEBUG

	for (auto& cls : evt_RigisterEvents) {
		DEBUG("[ " + cls.first + " ]")
			for (auto& evt : cls.second) {
				std::cout << "\t" << cls.first << " : " << evt << "\n";
			}
	}

#endif // _DEBUG
}

int EventListener::KeyMouseEvent::GenStateData() const
{
	int data = 0;
	data += ((int)FirstKey) * ((int)SecondKey + 2) * 4;
	data += NormKey * 1;
	data += (int)Mouse * 1;
	data += (int)Scr * 3;

	return data;
}
