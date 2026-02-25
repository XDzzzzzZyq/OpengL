/**
 * @file ImguiLayer.h
 * @brief Base class for all UI panels in the UI layer.
 * 
 * ImguiLayer provides the foundation for dockable UI panels (Viewport, Outliner,
 * TransformPanel, etc.). Each layer is a self-contained ImGui window that can
 * subscribe to events, render UI elements, and contain reusable widgets (ImguiItems).
 * 
 * Architecture:
 * - Layers are owned by ImguiManager
 * - Each layer has a unique name and ID for lookup
 * - Layers can be activated/deactivated independently
 * - Layers support docking into the main dockspace
 * - Communication: Layers read Context and emit Events
 * 
 * @note UI Layer: Layers must not mutate Context directly - use Events instead
 */

#pragma once

#include "ImGui/imgui.h"

#include "Context.h"
#include "Events.h"

#include "ImguiTheme.h"
#include "ImguiItem.h"

#include <unordered_map>
#define ACTIVE "ACTIVE LAYER"

/**
 * @brief Layer type identifiers for runtime type checking.
 * 
 * Used to identify specific layer types without RTTI. Each derived layer
 * should set uly_type to the corresponding enum value.
 */
enum ImLayerType
{
	NONE_UILAYER,         ///< Generic/unknown layer
	PARAS_UILAYER,        ///< Parameter control panel
	TOOLS_UILAYER,        ///< Tool settings panel
	VIEWPORT_UILAYER,     ///< 3D viewport with scene rendering
	OUTLINER_UILAYER,     ///< Scene hierarchy tree view
	SHADER_EDIT_UILAYER,  ///< Shader code editor
	MATERIAL_UILYER,      ///< Material properties viewer
	TRANSFORM_UILAYER,    ///< Transform editor (position/rotation/scale)
	RENDER_CONFIG_ULATER  ///< Render settings panel
};

/**
 * @brief Base class for all UI panels (dockable ImGui windows).
 * 
 * ImguiLayer represents a single UI panel that can be docked, resized, and
 * independently activated. Derived classes implement specific functionality:
 * - Viewport: Displays rendered scene
 * - Outliner: Shows scene hierarchy
 * - TransformPanel: Edits object transforms
 * - MaterialViewer: Manages material properties
 * 
 * Lifecycle:
 * 1. Construction - Layer created with name
 * 2. RegisterEvents() - Subscribe to relevant events
 * 3. RenderLayer() - Called each frame when active
 * 4. Destruction - Cleanup resources
 * 
 * Data Flow:
 * - Context (read) → Display state in UI
 * - User action → Emit Event via EventPool
 * 
 * @note Thread-safety: Not thread-safe. Must be used from main thread only.
 * @note Ownership: ImguiManager owns layers via shared_ptr
 */
class ImguiLayer
{
public:
	/**
	 * @brief Default constructor.
	 * @note Layer must be given a name before use
	 */
	ImguiLayer();
	
	/**
	 * @brief Constructs layer with specified name.
	 * @param name Unique layer name for lookup and display
	 */
	ImguiLayer(const std::string& name);
	
	/**
	 * @brief Virtual destructor for proper cleanup of derived classes.
	 */
	virtual ~ImguiLayer();
	
public:
	std::string uly_name;     ///< Layer name for display and lookup
	GLuint uly_ID = -1;       ///< Unique layer ID assigned by ImguiManager
	ImLayerType uly_type = NONE_UILAYER; ///< Layer type identifier
	std::vector<std::shared_ptr<ImguiItem>> item_list; ///< Widgets contained in this layer

private:
	mutable std::unordered_map<std::string, int> item_name_buffer; ///< Item name → index mapping

public:
	bool using_size = false;  ///< Whether to apply custom size constraints
	bool fixed_size = false;  ///< Whether layer size is fixed (non-resizable)
	
	/**
	 * @brief Size change tracking flags.
	 * @note TODO: Refactor to cleaner design pattern
	 */
	mutable bool is_size_changed = false;  ///< Current frame size change flag
	mutable bool is_size_changed_b = true; ///< Previous frame size change flag
	
	/**
	 * @brief Checks if layer was resized since last frame.
	 * @return true if layer was just resized
	 * @note TODO: Replace with event-based resize notification
	 */
	bool IsResized() const { return (is_size_changed == false) && (is_size_changed_b == true); }

	ImVec2 uly_size;   ///< Current layer size
	ImVec2 uly_size_b; ///< Previous layer size
	
	/**
	 * @brief Returns current layer size.
	 * @return Layer size in pixels
	 */
	ImVec2 GetLayerSize();

	ImVec2 uly_pos; ///< Layer position in screen space
	
	/**
	 * @brief Returns current layer position.
	 * @return Layer position in pixels
	 */
	ImVec2 GetLayerPos();

public:
	bool is_docking = true; ///< Whether layer can dock into dockspace
	
	/**
	 * @brief Adds a typed item to the layer.
	 * 
	 * Template factory for creating and adding items of specific types.
	 * 
	 * @tparam ItemType Derived type of ImguiItem
	 * @tparam Args Constructor argument types
	 * @param args Arguments forwarded to ItemType constructor
	 */
	template<class ItemType, class... Args>
	void PushItem(Args... args);
	
	/**
	 * @brief Adds an item to the layer.
	 * @param item Shared pointer to item to add
	 */
	void PushItem(std::shared_ptr<ImguiItem> item);
	
	/**
	 * @brief Quick-creates and adds an item by type.
	 * 
	 * Convenience method for standard item types (e.g., BUTTON_INP, FLOAT_INP).
	 * 
	 * @param type Item type to create
	 */
	void PushItem(ImItemType type);
	
	/**
	 * @brief Finds an item by name.
	 * @param name Item name
	 * @return Pointer to item, or nullptr if not found
	 */
	ImguiItem* FindImguiItem(const std::string& name) const;
	
	/**
	 * @brief Finds an item by index.
	 * @param id Item index in item_list (starts at 0)
	 * @return Pointer to item, or nullptr if invalid index
	 */
	ImguiItem* FindImguiItem(int id)const;

	bool uly_activate = true;      ///< Whether layer is active (receives events)
	bool uly_is_rendered = true;   ///< Whether layer is rendered (visible)

public:

	bool is_mouse_hovered = false; ///< Whether mouse is hovering over layer

public:
	bool uly_show_type = false; ///< Whether to display layer type in title
	
	/**
	 * @brief Registers layer's event subscriptions.
	 * 
	 * Override this to subscribe to relevant events (e.g., viewport resize,
	 * selection change). Called by ImguiManager during initialization.
	 * 
	 * @param evt EventPool for subscribing to events
	 */
	virtual void RegisterEvents(EventPool& evt) {};
	
	/**
	 * @brief Renders the layer's UI elements.
	 * 
	 * Override this to implement layer-specific rendering logic.
	 * Called every frame when layer is active and visible.
	 * 
	 * Typical implementation:
	 * 1. Begin ImGui window with layer name
	 * 2. Render items and custom UI
	 * 3. Handle user input
	 * 4. Emit events for state changes
	 * 5. End ImGui window
	 * 
	 * @param ctx Context for read-only access to scene/editor state
	 * @param evt EventPool for emitting events
	 * 
	 * @note Must not mutate Context - emit Events instead
	 */
	virtual void RenderLayer(const Context& ctx, EventPool& evt) {};
};

template<class ItemType, class... Args>
void ImguiLayer::PushItem(Args... args)
{
	std::shared_ptr<ItemType> item = std::make_shared<ItemType>(args...);
	PushItem(std::dynamic_pointer_cast<ImguiItem>(item));
}

#include <type_traits>

/**
 * @brief Concept constraining template parameter to ImguiLayer-derived types.
 * 
 * Used in template functions to ensure type safety when working with layers.
 * 
 * @tparam T Type to check
 */
template<typename T>
concept ImguiLayerType = std::is_base_of_v<ImguiLayer, T>;