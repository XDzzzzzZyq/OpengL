#pragma once
#define SPE_KEY_LEN 3 // alt ctrl shift
#define NORM_KEY_LEN 36

#include "GameObject.h"
#include "structs.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include<unordered_map>
#include<unordered_set>
#include<functional>

#define REGIST_EVENT(cls_event) std::bind(&cls_event , this); EventListener::REFLRigisterEvent(#cls_event)
#define REGIST_EVENT_STATIC(sta_event) &sta_event; EventListener::REFLRigisterEvent(#sta_event)

class EventListener
{
public:

	enum class SpecialKeys
	{
		NONE,
		SHIFT = 1,
		CTRL,
		ALT
	};

	enum class MouseStatus
	{
		NONE,
		LMB = 1,
		RMB,
		MMB
	};

	enum class ScrollDir
	{
		DOWN = -1,
		NONE,
		UP
	};

	//whole states including | 2 spe_keys | 1 norm_key | mouse |
						//   |	 3 + 1    |   1 + 36   | 1 + 3 |			

	struct KeyMouseEvent
	{

		SpecialKeys FirstKey{ SpecialKeys::NONE };
		SpecialKeys SecondKey{ SpecialKeys::NONE };
		int NormKey{ 0 };
		MouseStatus Mouse{ MouseStatus::NONE };
		ScrollDir Scr{ ScrollDir::NONE };
		bool is_update = false;
		bool is_pressed = false;

		bool operator==(const KeyMouseEvent& p) const {
			return FirstKey == p.FirstKey && SecondKey == p.SecondKey && NormKey == p.NormKey && Mouse == p.Mouse && Scr == p.Scr;
		}

		int GenStateData() const;
		void Debug() const {
#ifdef _DEBUG

			std::cout << (int)FirstKey << " "
				<< (int)SecondKey << " "
				<< (char)NormKey << " "
				<< (int)Mouse << " "
				<< (int)Scr << "\n";

#endif // DEBUG
		}

		struct hash_fn
		{
			std::size_t operator() (const KeyMouseEvent& inp) const
			{
				return inp.GenStateData();
			}
		};
	};

public:

	static KeyMouseEvent EVT_STATUS;
	static GLFWwindow* evt_window;

public:

	static int frame_count;

	static float random_float1;
	static float random_float2;
	static float random_float3;
	static float random_float4;

public:

	static bool is_key_pressed;
	static bool is_mouse_pressed;
	static bool is_key_pressed_b;
	static bool is_mouse_pressed_b;
	static bool is_key_changed;
	static bool IsMouseLeft()	{ return is_mouse_pressed_b == true && is_mouse_pressed == false; }    // true if mouse button on Left
	static bool IsMouseClick()	{ return is_mouse_pressed_b == false && is_mouse_pressed == true; }    // true if mouse button on Click
	static bool IsKeyLeft()		{ return is_key_pressed_b == true && is_key_pressed == false; }    // true if key button on Left
	static bool IsKeyClick()	{ return is_key_pressed_b == false && is_key_pressed == true; }    // true if key button on Click

public:

	static float mouse_x, mouse_y, mouse_b_x, mouse_b_y;
	static const float GetDeltaMouseX() { return mouse_x - mouse_b_x; }
	static const float GetDeltaMouseY() { return mouse_y - mouse_b_y; }
	static const glm::vec2 GetDeltaMouse() { return { mouse_x - mouse_b_x, mouse_y - mouse_b_y }; }
	static const glm::vec2 GetMousePos()   { return { mouse_x, mouse_y }; }

public:

	static std::vector<int> EVT_NK_LIST;
	static void PushNormKey(int _ID);
	static void PushNormKey(char _name);

public:

	static bool is_sprite_selected;
	static bool is_GOlist_changed;
	static bool is_selected_changed;
	static bool is_outliner_selected;
	static std::function<GameObject* (int)> GetActiveObject;
	static std::function<GameObject* (void)> GetActiveCamera;
	static int active_GO_ID;
	static std::vector<int> selec_list;
	static std::vector<int>parent_index_list;
	static OutlineData outline_list;
	static OutlineData* GetOutlineData() { return &outline_list; }
	static std::vector<int> GetParentRelatData() { return parent_index_list; }

	static GameObject* active_object;

public:

	enum ViewPortStatus
	{
		None,
		OnHover,
		OnClick
	};

	static glm::vec2 window_pos;
	static glm::vec2 viewport_offset;
	static bool is_in_viewport;
	static ViewPortStatus viewport_status;
	static void ReportGuizmoStatus(bool hover, bool click);

public:

	EventListener();
	EventListener(GLFWwindow* window);
	static void SetWindow(GLFWwindow* window);

	~EventListener();

public:

	std::unordered_map<KeyMouseEvent, std::function<void(void)>, KeyMouseEvent::hash_fn> EventList;
	static MouseStatus ListenMouseEvent(GLFWwindow* window);
	static SpecialKeys ListenSpecialKeyEvent(GLFWwindow* window, SpecialKeys ignor);
	static int ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist);

	static float scroll_dir;
	static bool is_scr_changed;
	static void scrollCall(GLFWwindow* window, double xoffset, double yoffset) { scroll_dir = (float)yoffset; }

	void UpdateEvent(GLFWwindow* window) const;

	static KeyMouseEvent GenIntEvent(int k1, int k2, int k3, int m, int scr);
	static KeyMouseEvent GenIntEvent(SpecialKeys k1, SpecialKeys k2, int k3, MouseStatus m, ScrollDir scr);

	void EventActivate();
	void Reset();

public:

	static std::vector<std::string> EVT_AVAIL_KEYS;
	static KeyMouseEvent ParseShortCut(const std::string& _shortcut);

	static std::unordered_map<std::string, std::unordered_set<std::string>> evt_RigisterEvents;
	static void REFLRigisterEvent(const std::string& _class_event);
	static void ShowEvents();

};
