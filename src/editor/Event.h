#pragma once
#define SPE_KEY_LEN 3 // alt ctrl shift
#define NORM_KEY_LEN 36

#include "ID.h"
#include "Events.h"
#include "Context.h"
#include "operator.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include<unordered_map>
#include<unordered_set>
#include<functional>
#include <typeindex>

#define REGIST_EVENT(cls_event) std::bind(&cls_event, this, std::placeholders::_1); EventCallback::REFLRigisterEvent(#cls_event)
#define REGIST_EVENT_STATIC(sta_event) &sta_event; EventCallback::REFLRigisterEvent(#sta_event)

class EventPool {
public:
	template<typename Event>
	using Handler = std::function<void(const Event&)>;

	template<typename Event>
	void subscribe(Handler<Event> handler) {
		auto& vec = handlers[typeid(Event)];
		vec.push_back(
			[h = std::move(handler)](const void* e) {
				h(*static_cast<const Event*>(e));
			}
		);
	}

	template<typename Event>
	void emit(const Event& event) {
		auto it = handlers.find(typeid(Event));
		if (it == handlers.end()) return;

		for (auto& fn : it->second) {
			fn(&event);
		}
	}

private:
	std::unordered_map<
		std::type_index,
		std::vector<std::function<void(const void*)>>
	> handlers;
};


class _ {
public:
	virtual void bind(EventPool& bus) = 0;
};


// TODO: Move to editor folder
class EventCallback
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
	static std::vector<int> parent_index_list;
	static std::vector<int> GetParentRelatData() { return parent_index_list; }

public:

	EventCallback();
	~EventCallback();

public:

	std::unordered_map<KeyMouseEvent, std::function<void(const SceneContext&)>, KeyMouseEvent::hash_fn> EventList;
	static MouseStatus ListenMouseEvent(GLFWwindow* window);
	static SpecialKeys ListenSpecialKeyEvent(GLFWwindow* window, SpecialKeys ignor);
	static int ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist);

	static float scroll_dir;
	static bool is_scr_changed;
	static void scrollCall(GLFWwindow* window, double xoffset, double yoffset) { scroll_dir = (float)yoffset; }

	void UpdateEvent(GLFWwindow* window) const;

	static KeyMouseEvent GenIntEvent(int k1, int k2, int k3, int m, int scr);
	static KeyMouseEvent GenIntEvent(SpecialKeys k1, SpecialKeys k2, int k3, MouseStatus m, ScrollDir scr);

	void EventActivate(const SceneContext& ctx);
	void Reset();

public:

	static std::vector<std::string> EVT_AVAIL_KEYS;
	static KeyMouseEvent ParseShortCut(const std::string& _shortcut);

	static std::unordered_map<std::string, std::unordered_set<std::string>> evt_RigisterEvents;
	static void REFLRigisterEvent(const std::string& _class_event);
	static void ShowEvents();

};
