#pragma once
#define SPE_KEY_LEN 3 // alt ctrl shift
#define NORM_KEY_LEN 36

#include "support.h"
#include "ShaderLib.h"

#include<unordered_map>
#include<unordered_set>
#include<functional>


//whole states including | 2 spe_keys | 1 norm_key | mouse |
					//   |	 3 + 1    |   1 + 36   | 1 + 3 |			

struct KeyMouseEvent
{

	int FirstKey{0};
	int SecondKey{0};
	int NormKey{0};
	int Mouse{0};
	int Scr{0};
	bool is_update = false;
	bool is_pressed = false;

	bool operator==(const KeyMouseEvent& p) const {
		return FirstKey == p.FirstKey && SecondKey == p.SecondKey && NormKey == p.NormKey && Mouse == p.Mouse && Scr == p.Scr;
	}

	int GenStateData() const;
	void Debug() const {
		std::cout << FirstKey << " "
			<< SecondKey << " "
			<< NormKey << " "
			<< Mouse << " "
			<< Scr << "\n";
	}

	struct hash_fn
	{
		std::size_t operator() (const KeyMouseEvent& inp) const
		{
			return inp.GenStateData();
		}
	};
};

#define REGIST_EVENT(cls_event) std::bind(&cls_event , this); EventListener::REFLRigisterEvent(#cls_event)

class EventListener
{
public:
	static KeyMouseEvent EVT_STATUS;
	static GLFWwindow* evt_window;
public:
	static bool is_key_pressed;
	static bool is_mouse_pressed;
	static bool is_key_pressed_b;
	static bool is_mouse_pressed_b;
	bool IsClick() const {
		return is_mouse_pressed_b == false && is_mouse_pressed == true;
	}
public:
	static double mouse_x, mouse_y, mouse_b_x, mouse_b_y;
	static const double GetDeltaMouseX() { return mouse_x - mouse_b_x; }
	static const double GetDeltaMouseY() { return mouse_y - mouse_b_y; }
	static const ImVec2 GetDeltaMouse() { return { (float)(mouse_x - mouse_b_x), (float)(mouse_y - mouse_b_y) }; }
public:
	static std::vector<int> EVT_NK_LIST;
	static void PushNormKey(int _ID);
	static void PushNormKey(char _name);
public:
	static bool is_spirit_selected;
	static bool is_GOlist_changed;
	static bool is_selected_changed;
	static bool is_outliner_selected;
	static std::function<void* (int)> GetActiveShader;
	static int active_GO_ID;
	static int pre_act_go_ID;
	static std::vector<int> selec_list;
	static std::vector<int>parent_index_list;
	static OutlineData outline_list;
	OutlineData* GetOutlineData() { return &outline_list; }
	std::vector<int> GetParentRelatData() { return parent_index_list; }

	static ShaderLib* active_shader;
public:
	static ImVec2 window_pos;
	static ImVec2 viewport_offset;
public:
	EventListener();
	EventListener(GLFWwindow* window);
	~EventListener();

public:
	std::unordered_map<KeyMouseEvent, std::function<void(void)>, KeyMouseEvent::hash_fn> EventList;
	int ListenMouseEvent(GLFWwindow* window) const;
	int ListenSpecialKeyEvent(GLFWwindow* window, int ignor) const;
	int ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist) const;

	static float scroll_dir;
	static bool is_scr_changed;
	static void scrollCall(GLFWwindow* window, double xoffset, double yoffset) {
		is_scr_changed = true;
		scroll_dir = yoffset;
	}

	void UpdateEvent(GLFWwindow* window) const;

	const KeyMouseEvent GenIntEvent(int k1, int k2, int k3, int m, int scr);

	void EventActivate();
	void Reset();
public:
	static std::vector<std::string> EVT_AVAIL_KEYS;
	static const KeyMouseEvent ParseShortCut(const std::string& _shortcut);

	static std::unordered_map<std::string, std::unordered_set<std::string>> evt_RigisterEvents;
	static void REFLRigisterEvent(const std::string& _class_event);
	static void ShowEvents();

};
