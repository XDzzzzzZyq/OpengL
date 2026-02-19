/**
 * @file Input.h
 * @brief Input handling system for keyboard, mouse, and viewport state.
 * 
 * Input provides a unified interface for querying user input state. It captures
 * keyboard keys, mouse buttons, mouse position/delta, scroll, and special keys
 * (Shift, Ctrl, Alt) into a consistent state snapshot updated each frame.
 * 
 * Architecture:
 * - Input state is captured via UpdateState() from GLFW each frame
 * - Current and previous frame states allow delta/change detection
 * - Static query methods provide convenient state checks
 * - Hotkey system supports special key combinations
 * 
 * Usage Pattern:
 * 1. UpdateState() at frame start - Snapshot current input
 * 2. Controllers/UI query state via static methods
 * 3. React to input changes (clicked, pressed, released)
 * 
 * @note Editor Layer: Input is part of the Editor system, not UI or Renderer
 * @note Thread-safety: Not thread-safe. Must be used from main thread only.
 */

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


/**
 * @brief Input handling system for keyboard, mouse, and viewport state.
 * 
 * Input captures and tracks user input state each frame, providing a consistent
 * interface for querying input changes. It supports:
 * - Keyboard: Individual keys + special modifiers (Shift, Ctrl, Alt)
 * - Mouse: Buttons (LMB, RMB, MMB), position, delta, scroll
 * - Viewport: Frame count, selection state (TODO: refactor out)
 * 
 * State Management:
 * - input_state: Current frame state
 * - input_state_b: Previous frame state
 * - Delta detection via state comparison
 * 
 * Common Patterns:
 * - IsKeyPressed(): Key held down this frame
 * - IsKeyClicked(): Key just pressed (transition)
 * - IsMousePressed(button): Mouse button held
 * - GetDeltaMouseX/Y(): Mouse movement since last frame
 * 
 * @note TODO: Viewport-specific state (frame_count, is_sprite_selected) should move to ViewportController
 */
class Input{

// KeyMouseState
public:

	/**
	 * @brief Special modifier keys (Shift, Ctrl, Alt).
	 * @note TODO: Use proper flag enum for multi-modifier support
	 */
	enum SpecialKeys
	{
		NONE = 0,       ///< No modifier
		SHIFT = 1 << 0, ///< Shift key
		CTRL = 1 << 1,  ///< Control key
		ALT = 1 << 2    ///< Alt key
	};

	/**
	 * @brief Mouse button identifiers.
	 */
	enum class MouseButtons : int
	{
		NONE,     ///< No button
		LMB = 1,  ///< Left mouse button
		RMB,      ///< Right mouse button
		MMB       ///< Middle mouse button
	};

	/**
	 * @brief Keyboard state snapshot.
	 * 
	 * Captures special keys (Shift/Ctrl/Alt) and normal key simultaneously.
	 * Supports hotkey matching (e.g., "Ctrl+S").
	 */
	struct KeyState
	{
		SpecialKeys special{ SpecialKeys::NONE }; ///< Active special modifiers (TODO: support multiple)
		int normal{ 0 };                          ///< Active normal key (1-36 mapping a-z and extras)
		
		/**
		 * @brief Equality comparison for hotkey matching.
		 * @param other State to compare against
		 * @return true if states match
		 */
		bool operator==(const KeyState& other) const
		{
			return special == other.special && normal == other.normal;
		}
	};

	/**
	 * @brief Mouse state snapshot.
	 * 
	 * Captures button press, position, and scroll delta.
	 */
	struct MouseState
	{
		MouseButtons button{ MouseButtons::NONE }; ///< Currently pressed button
		float mouse_x{ 0.0f };  ///< Mouse X position in screen space
		float mouse_y{ 0.0f };  ///< Mouse Y position in screen space
		float scroll_x{ 0.0f }; ///< Horizontal scroll delta
		float scroll_y{ 0.0f }; ///< Vertical scroll delta
	};

	/**
	 * @brief Parses a hotkey string into KeyState.
	 * 
	 * Supports format: "Ctrl+S", "Alt+F4", "Shift+A", etc.
	 * 
	 * @param hotkey Hotkey string to parse
	 * @return KeyState representing the hotkey
	 */
	static KeyState ParseKeyState(const std::string& hotkey);
	
	/**
	 * @brief Converts character to normal key index.
	 * 
	 * Maps 'a'-'z' (case-insensitive) to 1-26.
	 * 
	 * @param key Character to convert
	 * @return Key index (1-36)
	 */
	static constexpr int NormalKeyFromChar(char key)
	{
		const char lower = (key >= 'A' && key <= 'Z') ? static_cast<char>(key - 'A' + 'a') : key;
		return lower - 'a' + 1;
	}

// RandomState
public:

	/**
	 * @brief Random value state (purpose unclear).
	 * @note TODO: Document or remove if unused
	 */
	struct RandomState{
		float random_float1{ 0.0f };
		float random_float2{ 0.0f };
		float random_float3{ 0.0f };
		float random_float4{ 0.0f };
	};

// ViewportState
public:

	/**
	 * @brief Viewport-specific state tracking.
	 * @note TODO: This should be moved to ViewportController, not part of Input
	 */
	struct ViewportState {
		int frame_count{ 0 };              ///< Frame counter (TODO: move to editor context)
		bool is_sprite_selected{ false };  ///< Sprite selection flag (TODO: use SelectionManager)
		bool is_GOlist_changed{ false };   ///< Game object list change flag (TODO: use Event)
	};

// AllState
public:

	/**
	 * @brief Combined input state snapshot.
	 * 
	 * Aggregates all input state types for convenient frame capture.
	 */
	struct InputState {
		KeyState key{};            ///< Keyboard state
		MouseState mouse{};        ///< Mouse state
		RandomState random{};      ///< Random values (TODO: clarify usage)
		ViewportState viewport{};  ///< Viewport state (TODO: refactor out)
	};

public:

	/**
	 * @brief Current frame input state.
	 * @note TODO: Make private and provide getters
	 */
	static InputState input_state;
	
	/**
	 * @brief Previous frame input state.
	 * @note TODO: Make private
	 */
	static InputState input_state_b;

public:

	/**
	 * @brief Updates input state snapshot from GLFW.
	 * 
	 * Captures current keyboard, mouse, and window state from GLFW.
	 * Swaps current → previous for delta detection.
	 * 
	 * @param window GLFW window handle for input polling
	 * @note Must be called once per frame before input queries
	 */
	void UpdateState(GLFWwindow* window) const;

public:
	/**
	 * @brief Checks if any key was just pressed (transition from up to down).
	 * @return true if key clicked this frame
	 */
	static bool IsKeyClicked();
	
	/**
	 * @brief Checks if any key is currently held down.
	 * @return true if key pressed
	 */
	static bool IsKeyPressed();
	
	/**
	 * @brief Checks if a specific special key is pressed.
	 * @param spe_key Special key to check (Shift, Ctrl, Alt)
	 * @return true if special key is pressed
	 */
	static bool IsKeyPressed(SpecialKeys spe_key);
	
	/**
	 * @brief Checks if key was just released (transition from down to up).
	 * @return true if key released this frame
	 */
	static bool IsKeyLeft();

	/**
	 * @brief Checks if mouse scroll changed this frame.
	 * @return true if scroll delta is non-zero
	 */
	static bool IsMouseScrolled();

	/**
	 * @brief Checks if any mouse button was just clicked.
	 * @return true if mouse button clicked this frame
	 */
	static bool IsMouseClicked();
	
	/**
	 * @brief Checks if any mouse button is currently held down.
	 * @return true if mouse button pressed
	 */
	static bool IsMousePressed();
	
	/**
	 * @brief Checks if specific mouse button is pressed.
	 * @param button Mouse button to check (LMB, RMB, MMB)
	 * @return true if button is pressed
	 */
	static bool IsMousePressed(MouseButtons button);
	
	/**
	 * @brief Checks if mouse button was just released.
	 * @return true if button released this frame
	 */
	static bool IsMouseLeft();

	/**
	 * @brief Returns current mouse X position.
	 * @return Mouse X in screen space
	 */
	static float GetMousePosX();
	
	/**
	 * @brief Returns current mouse Y position.
	 * @return Mouse Y in screen space
	 */
	static float GetMousePosY();
	
	/**
	 * @brief Returns mouse X delta since last frame.
	 * @return Delta X in pixels
	 */
	static float GetDeltaMouseX();
	
	/**
	 * @brief Returns mouse Y delta since last frame.
	 * @return Delta Y in pixels
	 */
	static float GetDeltaMouseY();

	/**
	 * @brief Returns horizontal scroll delta.
	 * @return Scroll X delta
	 */
	static float GetScrollX();
	
	/**
	 * @brief Returns vertical scroll delta.
	 * @return Scroll Y delta
	 */
	static float GetScrollY();

	/**
	 * @brief Checks if game object list changed.
	 * @return true if object list modified
	 * @note TODO: Replace with Event-based notification
	 */
	static bool IsGOListChanged();
	
	/**
	 * @brief Checks if a sprite is selected.
	 * @return true if sprite selected
	 * @note TODO: Use SelectionManager instead
	 */
	static bool IsSpriteSelected();
	
	/**
	 * @brief Resets frame counter.
	 * @param count New frame count value
	 * @note TODO: Move to EditorContext
	 */
	static void ResetFrameCount(int count);
	
	/**
	 * @brief Returns current frame count.
	 * @return Frame count
	 * @note TODO: Move to EditorContext
	 */
	static int GetFrameCount();
};
