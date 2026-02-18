/**
 * @file ImguiItem.h
 * @brief Base class for reusable UI widgets within layers.
 * 
 * ImguiItem provides the foundation for all UI widgets (buttons, sliders, text inputs,
 * viewports, etc.) that can be embedded in ImguiLayer panels. Items are self-contained
 * UI elements with consistent interface for rendering and value access.
 * 
 * Widget Types:
 * - Input: Text, Float, Int, RGB/RGBA, Vec2/Vec3/Vec4
 * - Button: Clickable buttons with callbacks
 * - Output: Text display, Image/Texture display, Viewport
 * - Selection: Dropdown/combo boxes
 * 
 * Architecture:
 * - Items are owned by their parent ImguiLayer
 * - Each item has a unique name and ID within its layer
 * - Items render themselves via RenderItem()
 * - Parameter items expose value via GetPara()
 * - Button items execute callbacks via CallButtonFunc()
 * 
 * @note UI Layer: Items are presentation-only and must not directly mutate Scene
 */

#pragma once
#include "ImGui/imgui.h"

#include "Parameters.h"

#include <iostream>
#include <optional>
#include <functional>
#include <type_traits>
#include <cstdarg>

#include "macros.h"

/**
 * @brief Widget type identifiers for runtime type checking.
 * 
 * Each item type has a corresponding enum value. Used for quick type
 * identification without RTTI.
 */
enum ImItemType
{
	NONE_INP,      ///< Generic/unknown item
	TEXT_INP,      ///< Text input field
	FLOAT_INP,     ///< Float slider/input
	INT_INP,       ///< Integer slider/input
	RGB_INP,       ///< RGB color picker (3 channels)
	RGBA_INP,      ///< RGBA color picker (4 channels)
	VEC2_INP,      ///< 2D vector input
	VEC3_INP,      ///< 3D vector input (e.g., position)
	VEC4_INP,      ///< 4D vector input
	BUTTON_INP,    ///< Clickable button
	BOOL_INP,      ///< Boolean checkbox
	SELECT_INP,    ///< Dropdown selection
	IMAGE_OUTP,    ///< Image/texture display
	TEXT_OUTP,     ///< Text label/output
	VIEWPORT_OUTP  ///< Viewport for rendered scene
};

/**
 * @brief Base class for all UI widgets (buttons, sliders, inputs, etc.).
 * 
 * ImguiItem represents a single reusable UI widget that can be added to any
 * ImguiLayer. Items handle their own rendering via ImGui calls and provide
 * a consistent interface for value access and callbacks.
 * 
 * Item Lifecycle:
 * 1. Construction - Item created with type and name
 * 2. Configuration - Set size, callbacks, initial values
 * 3. RenderItem() - Called each frame when parent layer is visible
 * 4. Destruction - Cleanup resources
 * 
 * Common Item Patterns:
 * - Parameter items: Expose GetPara() for value access
 * - Button items: Execute ButtonFunc callback on press
 * - Viewport items: Support texture ID and UV coordinate updates
 * 
 * @note Thread-safety: Not thread-safe. Must be used from main thread only.
 * @note Ownership: Items are owned by their parent ImguiLayer via shared_ptr
 */
class ImguiItem
{
private:

public:
	/**
	 * @brief Default constructor.
	 * @note Item must be configured before use
	 */
	ImguiItem();
	
	/**
	 * @brief Constructs item with type and name.
	 * @param type Item type identifier
	 * @param name Unique item name for lookup and display
	 */
	ImguiItem(ImItemType type, const std::string& name);
	
	/**
	 * @brief Virtual destructor for proper cleanup of derived classes.
	 */
	virtual ~ImguiItem();
	
public:
	mutable bool using_size = false;    ///< Whether to apply custom size
	mutable bool fixed_size = false;    ///< Whether item size is fixed
	mutable bool show_tag_name = false; ///< Whether to display tag name
	ImVec2 uitm_size;                   ///< Item size in pixels
	
public:
	mutable bool is_activated = false; ///< Whether item is currently active/focused
	
public:
	ImItemType uitm_type = NONE_INP;   ///< Item type identifier
	mutable std::string uitm_name = ""; ///< Item name for display and lookup
	mutable std::string notagname = ""; ///< Display name without tag prefix
	mutable int uitm_id = -1;           ///< Unique item ID within layer
	
public:
	/**
	 * @brief Renames the item.
	 * @param name New item name
	 */
	void Rename(const std::string& name) const;
	
	/**
	 * @brief Returns the tag name for display.
	 * @return Tag name string (includes type prefix if enabled)
	 */
	std::string GetTagName()const;
	
	/**
	 * @brief Enables tag name display.
	 */
	void EnableTagName() const;
	
	/**
	 * @brief Disables tag name display.
	 */
	void DisableTagName() const;
	
	/**
	 * @brief Returns item name as C-string.
	 * @return C-string pointer for ImGui functions
	 */
	const char* GetCharName() const { return uitm_name.c_str(); }
	
public:
	/**
	 * @brief Renders the item's UI.
	 * 
	 * Override this to implement item-specific rendering logic using ImGui calls.
	 * Called every frame when parent layer is visible.
	 * 
	 * @note Default implementation logs error. All derived classes must override.
	 */
	virtual void RenderItem() const { DEBUG("no render function overrided"); return; }

	/**
	 * @brief Returns pointer to parameter value.
	 * 
	 * Only applicable to parameter input items (sliders, text inputs, etc.).
	 * 
	 * @return Pointer to Parameters, or nullptr if item is not a parameter input
	 * @note Override in parameter input classes
	 */
	virtual Parameters* GetPara() { DEBUG(uitm_name + " is not a paraInp"); return nullptr; }

	/**
	 * @brief Callback function executed when button is pressed.
	 * 
	 * Set this to a custom lambda or function for button items.
	 * Default is no-op.
	 */
	mutable std::function<void(void)> ButtonFunc = [] {};
	
	/**
	 * @brief Executes the button callback.
	 * 
	 * Only applicable to button items. Call this when button is pressed.
	 * 
	 * @note Override in button classes
	 */
	virtual void CallButtonFunc() const { DEBUG(uitm_name + " is not a button"); return; }
	
	mutable bool is_button_pressed; ///< Whether button is currently pressed

	/**
	 * @brief Sets variable argument list for text output items.
	 * 
	 * Used by text output items to support formatted string display.
	 * 
	 * @param length Number of arguments
	 * @param ... Variable arguments for formatting
	 * @note Override in text output classes
	 */
	virtual void SetArgsList(int length, ...) const { DEBUG(uitm_name + " is not a text");	return; }
	
	/**
	 * @brief Sets argument list from float array for text output.
	 * @param length Array length
	 * @param f1 Float array pointer
	 * @note Override in text output classes
	 */
	virtual void SetArgsList(int length, float* f1) const { DEBUG(uitm_name + " is not a text"); return; }

	/**
	 * @brief Updates UV coordinates for viewport items.
	 * 
	 * Used by viewport items to specify texture sampling region.
	 * 
	 * @param min Minimum UV coordinate (bottom-left)
	 * @param max Maximum UV coordinate (top-right)
	 * @note Override in viewport classes
	 */
	virtual void ResetUV(const ImVec2& min, const ImVec2& max) { DEBUG(uitm_name + " is not a viewport"); return; }
	
	/**
	 * @brief Updates size for viewport items.
	 * @param size New viewport size in pixels
	 * @note Override in viewport classes
	 */
	virtual void ResetSize(const ImVec2& size) { DEBUG(uitm_name + " is not a viewport"); return; }
	
	/**
	 * @brief Updates texture/framebuffer ID for viewport items.
	 * @param id OpenGL texture/framebuffer ID to display
	 * @note Override in viewport classes
	 */
	virtual void ResetBufferID(int id) { DEBUG(uitm_name + " is not a viewport");	return; }


};

#include "operator.h"

/**
 * @brief Utility functions for ImGui item operations.
 */
namespace Item {

	/**
	 * @brief Checks if mouse position is inside a window region.
	 * 
	 * Used for hit-testing and hover detection.
	 * 
	 * @param size Window/item size
	 * @param mouse_pos Mouse position in screen space
	 * @return true if mouse is inside the region
	 */
	inline bool is_inside(const ImVec2 size, const ImVec2 mouse_pos) {
		const ImVec2 window_pos = ImGui::GetWindowPos() - ImGui::GetMainViewport()->Pos;
		return window_pos < mouse_pos && mouse_pos < window_pos + size;
	}
}

#include <type_traits>

/**
 * @brief Concept constraining template parameter to ImguiItem-derived types.
 * 
 * Used in template functions to ensure type safety when working with items.
 * 
 * @tparam T Type to check
 */
template<typename T>
concept ImguiItemType = std::is_base_of_v<ImguiItem, T>;