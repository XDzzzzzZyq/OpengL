#pragma once
#define SPE_KEY_LEN 3 // alt ctrl shift
#define NORM_KEY_LEN 36

#include "ID.h"
#include "Context.h"
#include "operator.h"

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <string>

#define REGIST_EVENT(cls_event) std::bind(&cls_event, this, std::placeholders::_1); Input::REFLRigisterEvent(#cls_event)
#define REGIST_EVENT_STATIC(sta_event) &sta_event; Input::REFLRigisterEvent(#sta_event)


class Input{

// KeyMouseState
public:

	// TODO: use flag
	enum SpecialKeys
	{
		NONE = 0,
		SHIFT = 1 << 0,
		CTRL = 1 << 1,
		ALT = 1 << 2
	};

	enum class MouseButtons : int
	{
		NONE,
		LMB = 1,
		RMB,
		MMB
	};

	//whole states including | 2 spe_keys | 1 norm_key | mouse |
						//   |	 3 + 1    |   1 + 36   | 1 + 3 |			

	struct KeyState
	{
		// TODO: use flag to capture all special keys
		SpecialKeys special{ SpecialKeys::NONE };
		int normal{ 0 };
		bool operator==(const KeyState& other) const
		{
			return special == other.special && normal == other.normal;
		}
	};

	struct MouseState
	{
		MouseButtons button{ MouseButtons::NONE };

		float mouse_x{ 0.0f };
		float mouse_y{ 0.0f };
		float scroll_x{ 0.0f };
		float scroll_y{ 0.0f };
	};

	static KeyState ParseKeyState(const std::string& hotkey);
	static constexpr int NormalKeyFromChar(char key)
	{
		const char lower = (key >= 'A' && key <= 'Z') ? static_cast<char>(key - 'A' + 'a') : key;
		return lower - 'a' + 1;
	}

// RandomState
public:

	struct RandomState{
		float random_float1{ 0.0f };
		float random_float2{ 0.0f };
		float random_float3{ 0.0f };
		float random_float4{ 0.0f };
	};

// ViewportState
public:

	// TODO: better design
	struct ViewportState {
		int frame_count{ 0 };
		bool is_sprite_selected{ false };
		bool is_GOlist_changed{ false };
	};

// AllState
public:

	struct InputState {
		KeyState key{};
		MouseState mouse{};
		RandomState random{};
		ViewportState viewport{};
	};

public:

	// TODO: make it private
	static InputState input_state;
	static InputState input_state_b;

public:

	// Generate input state snapshot
	void UpdateState(GLFWwindow* window) const;

public:
	static bool IsKeyClicked();
	static bool IsKeyPressed();
	static bool IsKeyPressed(SpecialKeys spe_key);
	static bool IsKeyLeft();

	static bool IsMouseScrolled();

	static bool IsMouseClicked();
	static bool IsMousePressed();
	static bool IsMousePressed(MouseButtons button);
	static bool IsMouseLeft();

	static float GetMousePosX();
	static float GetMousePosY();
	static float GetDeltaMouseX();
	static float GetDeltaMouseY();

	static float GetScrollX();
	static float GetScrollY();

	// TODO: better design
	static bool IsGOListChanged();
	static bool IsSpriteSelected();
	static void ResetFrameCount(int count);
	static int GetFrameCount();
};
