/**
 * @file Context.h
 * @brief Context system providing explicit data flow across Renderer, Editor, and UI layers.
 * 
 * The Context system enables decoupled communication by aggregating read-only views of
 * application state. It enforces architectural boundaries: Editor owns and mutates Context,
 * while Renderer and UI consume it immutably.
 */

#pragma once

#include "ID.h"
#include "SelectionManager.h"
#include "Events.h"
#include "RenderConfigs.h"

/**
 * @brief Provides read-only access to the active scene and its objects.
 * 
 * SceneContext is the primary interface for querying scene state. It returns
 * const pointers to enforce immutability from consumers (Renderer, UI).
 * 
 * @note Thread-safety: Not thread-safe. Must be accessed from main thread only.
 */
class SceneContext
{
public:
	UID* active_scene{ nullptr }; ///< Root of the active scene hierarchy

	/**
	 * @brief Sets the active scene.
	 * @param scene Pointer to the scene root to activate
	 */
	void UseScene(UID* scene) { active_scene = scene; };

	/* Read-only accessors */
	
	/**
	 * @brief Returns the currently active camera.
	 * @return Const pointer to active Camera, or nullptr if none active
	 */
	const ObjectID* GetActiveCamera() const;
	
	/**
	 * @brief Returns the currently active environment for IBL.
	 * @return Const pointer to active Environment, or nullptr if none active
	 */
	const ObjectID* GetActiveEnvironment() const;
	
	/**
	 * @brief Returns the post-processing stack for a given target.
	 * @param _tar Target index for post-processing stack
	 * @return Const pointer to post-processing object
	 */
	const ObjectID* GetPPS(int _tar) const;

	/**
	 * @brief Returns all renderable objects in the scene.
	 * @return Vector of const pointers to scene objects
	 * @note Used by Renderer to iterate objects for draw calls
	 */
	const std::vector<const ObjectID*> GetObjectIDs() const;
};

/**
 * @brief Stores editor-specific state (selections, tools, etc.).
 * 
 * EditorContext holds state that belongs to the Editor layer but not to the Scene.
 * Examples include current selection, undo history, clipboard, active tool.
 * 
 * @note Ownership: Created and mutated by Editor layer
 */
class EditorContext
{
public:
	SelectionManager<ObjectID> selections; ///< Currently selected objects
};

/**
 * @brief Links to rendering configuration without owning it.
 * 
 * RenderContext provides access to RenderConfigs via non-owning pointer.
 * This allows Renderer to read settings without coupling to config ownership.
 * 
 * @note Ownership: RenderConfigs is owned externally (Editor or Application)
 */
class RenderContext
{
private:
	RenderConfigs* config{ nullptr }; ///< Non-owning pointer to render settings
	
public:
	/**
	 * @brief Returns pointer to render configuration.
	 * @return Non-owning pointer to RenderConfigs
	 */
	RenderConfigs* GetConfig() const { return config; }
	void UseConfig(RenderConfigs* _config) { config = _config; };
};

/**
 * @brief Unified context container aggregating all context types.
 * 
 * Context is the single source of truth for queryable application state.
 * It is passed across layers to enable decoupled data access:
 * - Editor mutates Context
 * - Renderer receives const Context& (read-only)
 * - UI reads Context for display
 * 
 * @note Architecture: Context enforces one-way data flow and prevents hidden coupling
 */
class Context {
public:
	SceneContext scene{};   ///< Scene graph read-only view
	EditorContext editor{}; ///< Editor state (selections, etc.)
	RenderContext render{}; ///< Render settings accessor

public:
	/**
	 * @brief Initializes the context system.
	 * @param pool EventPool for subscribing to context-related events
	 */
	void Init(EventPool& pool);
};

