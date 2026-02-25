/**
 * @file ImguiManager.h
 * @brief UI system coordinator managing ImGui lifecycle, layers, and menus.
 * 
 * ImguiManager is the central UI orchestrator that coordinates all UI panels (layers)
 * and menus. It manages the ImGui context lifecycle, docking system, and provides
 * lookup services for UI components.
 * 
 * Architecture:
 * - Owns all ImguiLayer and ImguiMenu instances
 * - Manages ImGui context initialization and rendering
 * - Provides name-based lookup for layers and items
 * - Coordinates event registration across UI components
 * - Handles multi-viewport and docking configuration
 * 
 * @note UI Layer: ImguiManager is read-only with respect to Context and emits Events
 */

#pragma once

#include "ImGui/imgui.h"

#include "Context.h"
#include "Events.h"

#include "ImguiMenu.h"
#include "ImguiLayer.h"
#include "ImguiItem.h"

#include <vector>
#include <unordered_map>

class Window; ///< Forward declaration — see app/Window.h

/*#define ParaUpdate ParaUpdate*/

/**
 * @brief Central UI coordinator managing ImGui lifecycle, layers, and menus.
 * 
 * ImguiManager orchestrates the UI system by:
 * 1. Initializing ImGui context and backends (GLFW + OpenGL3)
 * 2. Managing UI panels (ImguiLayer) and menus (ImguiMenu)
 * 3. Coordinating event subscriptions across UI components
 * 4. Rendering all active UI elements each frame
 * 5. Providing lookup and access to UI components by name or ID
 * 
 * UI Rendering Flow:
 * - NewFrame() - Prepare ImGui for new frame
 * - RenderUI() - Render all active layers and menus
 * - Destructor - Cleanup ImGui resources
 * 
 * Component Management:
 * - Layers: Dockable UI panels (Viewport, Outliner, etc.)
 * - Menus: Top-level menu bar items
 * - Items: Widgets within layers (buttons, sliders, etc.)
 * 
 * @note Thread-safety: Not thread-safe. Must be used from main thread only.
 * @note Ownership: ImguiManager owns all Layer and Menu instances via shared_ptr
 */
class ImguiManager
{
private:
	std::vector<std::shared_ptr<ImguiLayer>> layer_list; ///< Owned UI panels (Viewport, Outliner, etc.)
	std::vector<std::shared_ptr<ImguiMenu>> menu_list;   ///< Owned menu bar components
	
	ImGuiIO* io = nullptr;        ///< ImGui I/O configuration and state
	ImGuiStyle* m_style= nullptr; ///< ImGui visual style settings

	mutable std::unordered_map<std::string, int> layer_name_buffer;  ///< Layer name → index mapping
	mutable std::unordered_map<std::string, int> menu_name_buffer;   ///< Menu name → index mapping

public:
	static bool is_prefW_open; ///< Preferences window open state

public:

	/**
	 * @brief Constructs and fully initializes the ImguiManager.
	 *
	 * Takes Window& to enforce that the GLFW/GL context (steps 1–3) is active
	 * before ImGui backend initialization. Creates the ImGui context internally.
	 * Sets up:
	 * - ImGui context with GLFW + OpenGL3 backends
	 * - Configuration flags (docking, viewports, etc.)
	 * - UI theme and style
	 * - Default layers and menus
	 * - Event subscriptions for all UI components
	 *
	 * @param evt EventPool for subscribing to UI-related events
	 * @param w   Active Window — used for GLFW backend initialization
	 */
	ImguiManager(EventPool& evt, Window& w);

	/**
	 * @brief Shuts down ImGui backends and destroys context.
	 */
	~ImguiManager();

	ImguiManager(const ImguiManager&) = delete;
	ImguiManager& operator=(const ImguiManager&) = delete;

	/**
	 * @brief Debug utility for printing UI component hierarchy.
	 * @note Development/diagnostic use only
	 */
	void _debug() const;

public:
	/**
	 * @brief Sets an ImGui configuration flag.
	 * @param flag Configuration flag to enable (e.g., docking, viewports)
	 */
	void SetConfigFlag(ImGuiConfigFlags_ flag) const { io->ConfigFlags |= flag; };
	
	/**
	 * @brief Sets an ImGui backend flag.
	 * @param flag Backend flag to enable (e.g., renderer features)
	 */
	void SetBackendFlag(ImGuiBackendFlags_ flag) const { io->BackendFlags |= flag; };
	
	/**
	 * @brief Returns ImGui I/O interface.
	 * @return Pointer to ImGuiIO for configuration access
	 */
	ImGuiIO* GetIO()const { return io; }
	
	/**
	 * @brief Returns ImGui style settings.
	 * @return Pointer to ImGuiStyle for theme customization
	 */
	ImGuiStyle* GetStyle()const { return m_style; }

	/**
	 * @brief Sets up default viewport layout configuration.
	 * @note Called during initialization to establish docking layout
	 */
	void DefultViewports();
	
	/**
	 * @brief Registers all layer event subscriptions.
	 * 
	 * Iterates through all layers and calls RegisterEvents() on each,
	 * allowing them to subscribe to relevant events.
	 * 
	 * @param evt EventPool for event subscription
	 */
	void RegisterLayerEvents(EventPool& evt);
	
	/**
	 * @brief Registers all menu event subscriptions.
	 * @param evt EventPool for event subscription
	 */
	void RegistarMenuEvents(EventPool& evt);

public:
	/**
	 * @brief Prepares ImGui for a new frame.
	 * 
	 * Must be called at the start of each frame before any ImGui calls.
	 * Updates input state, timing, and prepares rendering context.
	 * 
	 * @note Call sequence: NewFrame() → RenderUI() → backend render
	 */
	void NewFrame() const;
	
	/**
	 * @brief Renders all active UI layers and menus.
	 * 
	 * Rendering flow:
	 * 1. Begin main dockspace
	 * 2. Render menu bar
	 * 3. Render all active layers (if uly_activate && uly_is_rendered)
	 * 4. Finalize ImGui rendering
	 * 
	 * @param ctx Context for read-only access to scene/editor state
	 * @param evt EventPool for emitting UI events
	 * @param rend Enable rendering (default: true)
	 * 
	 * @note UI reads Context but never mutates it
	 * @note User actions emit Events via evt
	 */
	void RenderUI(const Context& ctx, EventPool& evt, bool rend = true);

public:
	/**
	 * @brief Adds a menu to the menu bar.
	 * @param _menu Shared pointer to menu to add
	 */
	void PushImguiMenu(std::shared_ptr<ImguiMenu> _menu);
	
	/**
	 * @brief Creates and registers a new menu.
	 * @param name Menu name for lookup and display
	 * @return Shared pointer to created menu
	 */
	std::shared_ptr<ImguiMenu> CreateImguiMenu(std::string name);

	/**
	 * @brief Finds a menu by name.
	 * @param name Menu name
	 * @return Pointer to menu, or nullptr if not found
	 */
	ImguiMenu* FindImguiMenu(const std::string& name) const;
	
	/**
	 * @brief Finds a menu item within a menu.
	 * @param menu Parent menu name
	 * @param submenu Menu item name
	 * @return Pointer to menu item, or nullptr if not found
	 */
	ImguiMenuItem* FindImguiMenuItem(const std::string& menu, const std::string& submenu) const;

public:
	/**
	 * @brief Adds a layer (UI panel) to the manager.
	 * @param layer Shared pointer to layer to add
	 * @note Layer is registered in name buffer for lookup
	 */
	void PushImguiLayer(std::shared_ptr<ImguiLayer> layer);
	
	/**
	 * @brief Creates and registers a new typed layer.
	 * 
	 * Template factory method for creating specific layer types
	 * (e.g., Viewport, Outliner, TransformPanel).
	 * 
	 * @tparam LayerType Derived type of ImguiLayer
	 * @param name Layer name for lookup and display
	 * @return Shared pointer to created layer
	 */
	template<class LayerType>
	std::shared_ptr<LayerType> CreateImguiLayer(std::string name);

	/**
	 * @brief Finds a layer by name.
	 * @param name Layer name
	 * @return Pointer to layer, or nullptr if not found
	 */
	ImguiLayer* FindImguiLayer(const std::string& name)const;
	
	/**
	 * @brief Finds a layer by index ID.
	 * @param id Layer index in layer_list
	 * @return Pointer to layer, or nullptr if invalid ID
	 */
	ImguiLayer* FindImguiLayer(int id)const;
	
	/**
	 * @brief Finds a layer and casts to derived type.
	 * 
	 * Type-safe helper for accessing specific layer types without
	 * manual casting.
	 * 
	 * @tparam T Derived layer type (e.g., Viewport)
	 * @tparam Args Argument types for FindImguiLayer
	 * @param args Arguments forwarded to FindImguiLayer
	 * @return Pointer to layer cast to T, or nullptr
	 * 
	 * @note No runtime type checking. Caller must ensure correct type.
	 */
	template<ImguiLayerType T, typename... Args>
	T* FindImguiLayerAs(Args&&... args) const
	{
		ImguiLayer* base = FindImguiLayer(std::forward<Args>(args)...);
		return static_cast<T*>(base);
	}

	/**
	 * @brief Finds an item within a layer by name.
	 * @param layer Layer name
	 * @param name Item name
	 * @return Pointer to item, or nullptr if not found
	 */
	ImguiItem* FindImguiItem(const std::string& layer, const std::string& name) const;
	
	/**
	 * @brief Finds an item within a layer by layer ID and item name.
	 * @param id Layer ID
	 * @param name Item name
	 * @return Pointer to item, or nullptr if not found
	 */
	ImguiItem* FindImguiItem(int id, const std::string& name) const;
	
	/**
	 * @brief Finds an item by layer ID and item ID.
	 * @param id Layer ID
	 * @param item_id Item ID within layer
	 * @return Pointer to item, or nullptr if not found
	 */
	ImguiItem* FindImguiItem(int id, int item_id) const;
	
	/**
	 * @brief Finds an item and casts to derived type.
	 * 
	 * Type-safe helper for accessing specific item types (e.g., Button, Slider).
	 * 
	 * @tparam T Derived item type
	 * @tparam Args Argument types for FindImguiItem
	 * @param args Arguments forwarded to FindImguiItem
	 * @return Pointer to item cast to T, or nullptr
	 */
	template<ImguiItemType T, typename... Args>
	T* FindImguiItemAs(Args&&... args) const
	{
		ImguiItem* base = FindImguiItem(std::forward<Args>(args)...);
		return static_cast<T*>(base);
	}

public:
	/**
	 * @brief Activates (shows) a layer by name.
	 * 
	 * Sets uly_activate = true, making the layer visible and renderable.
	 * 
	 * @param name Layer name to activate
	 */
	void ActivateLayer(const std::string& name);

public:
	/**
	 * @brief Sets the callback function for a button item.
	 * 
	 * Assigns a lambda or function to be called when the button is pressed.
	 * 
	 * @param ly_name Layer name containing the button
	 * @param it_name Item name of the button
	 * @param func Callback function to execute on button press
	 */
	void SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func);
	
	/**
	 * @brief Retrieves parameter value from a parameter input item.
	 * 
	 * Used to read values from sliders, text inputs, color pickers, etc.
	 * 
	 * @param ly_name Layer name containing the parameter item
	 * @param it_name Item name of the parameter input
	 * @return Pointer to Parameters, or nullptr if item is not a parameter input
	 */
	Parameters* GetParaValue(const std::string& ly_name, const std::string& it_name);

public:
	/**
	 * @brief Callback invoked when parameters are updated.
	 * 
	 * This hook allows external systems to react to parameter changes.
	 * Set this to a custom function to handle updates globally.
	 * 
	 * @note Default: No-op lambda
	 */
	mutable std::function<void(void)> ParaUpdate = [] {};

};

template<class LayerType>
std::shared_ptr<LayerType> ImguiManager::CreateImguiLayer(std::string name)
{
	std::shared_ptr<LayerType> layer = std::make_shared<LayerType>(name);
	PushImguiLayer(std::dynamic_pointer_cast<ImguiLayer>(layer));

	return layer;
}

