#pragma once
#define SPE_KEY_LEN 3 // alt ctrl shift
#define NORM_KEY_LEN 36

#include "ID.h"
#include "Context.h"
#include "operator.h"

#include<unordered_map>
#include<functional>
#include <typeindex>

#define REGIST_EVENT(cls_event) std::bind(&cls_event, this, std::placeholders::_1); Input::REFLRigisterEvent(#cls_event)
#define REGIST_EVENT_STATIC(sta_event) &sta_event; Input::REFLRigisterEvent(#sta_event)


class Input{

// KeyMouseState
public:

	// TODO: use flag
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

	//whole states including | 2 spe_keys | 1 norm_key | mouse |
						//   |	 3 + 1    |   1 + 36   | 1 + 3 |			

	struct KeyState
	{
		// TODO: use flag to capture all special keys
		SpecialKeys FirstKey{ SpecialKeys::NONE };
		SpecialKeys SecondKey{ SpecialKeys::NONE };
		int NormKey{ 0 };
	};

	struct MouseState
	{
		MouseStatus button{ MouseStatus::NONE };

		float mouse_x{ 0.0f };
		float mouse_y{ 0.0f };
		float scroll_x{ 0.0f };
		float scroll_y{ 0.0f };
	};

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
		bool is_selected_changed{ false };
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
	static bool IsKeyLeft();

	static bool IsMouseScrolled();

	static bool IsMouseClicked();
	static bool IsMousePressed();
	static bool IsMouseLeft();

	static float GetMousePosX();
	static float GetMousePosY();
	static float GetDeltaMouseX();
	static float GetDeltaMouseY();

	static float GetScrollX();
	static float GetScrollY();

	// TODO: better design
	static bool IsSelectedChanged();
	static bool IsGOListChanged();
	static bool IsSpriteSelected();
	static void ResetFrameCount(int count);
	static int GetFrameCount();
};
