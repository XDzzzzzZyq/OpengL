/**
 * @file Controllers.h
 * @brief Base controller architecture for event-driven application logic.
 * 
 * Controllers contain Editor-layer logic that responds to Events from the UI
 * and manipulates scene state accordingly. This file defines the base controller
 * interface and a centralized manager for controller registration.
 * 
 * Architecture:
 * - Controllers bind to EventPool on initialization to subscribe to relevant events
 * - Each controller handles a specific domain (camera, viewport, shaders, etc.)
 * - ControllerManager owns and orchestrates all controllers
 * - Controllers operate on Scene/Context but never directly call OpenGL
 * 
 * Design Pattern:
 * - Controllers bridge UI events → Scene mutations
 * - State flows: UI → Event → Controller → Scene → Context → Renderer
 * - Controllers MAY emit events to notify other systems
 * - Controllers MUST NOT depend on UI or Renderer internals
 * 
 * @note Controllers are Editor-layer components and must not leak into Renderer.
 * @note Each controller type is registered once via RTTI-based lookup.
 */

#pragma once
#include "Events.h"

#include <unordered_map>
#include <typeindex>
#include <memory>

/**
 * @brief Abstract base class for all controllers in the Editor layer.
 * 
 * Controllers encapsulate domain-specific application logic that responds to
 * Events emitted by the UI or other systems. They subscribe to relevant events
 * via the EventPool during initialization and update scene state accordingly.
 * 
 * Lifecycle:
 * 1. Controller is instantiated by ControllerManager
 * 2. bind() is called to subscribe to EventPool
 * 3. Event handlers react to user actions or state changes
 * 4. Controller mutates scene objects via direct access or additional events
 * 
 * Design Constraints:
 * - Controllers MUST NOT hold GPU resources
 * - Controllers MUST NOT directly call OpenGL functions
 * - Controllers SHOULD be stateless or minimize internal state
 * - Controllers communicate via Events for cross-domain coordination
 * 
 * @note Pure abstract base class - all controllers must implement bind().
 */
class Controllers
{
public:
	/**
	 * @brief Subscribe to relevant events in the EventPool.
	 * 
	 * This method is called once during controller initialization to establish
	 * event subscriptions. Controllers should subscribe to all events they need
	 * to handle their domain logic (e.g., CameraController subscribes to mouse drag).
	 * 
	 * Event handlers typically:
	 * - Validate event data
	 * - Update scene objects (Transform, Material, etc.)
	 * - Emit secondary events to notify other systems
	 * 
	 * @param pool The EventPool to bind subscriptions to
	 * 
	 * @note This is called before any events are emitted, ensuring handlers are ready.
	 * @note Subscriptions persist for the lifetime of the controller.
	 */
	virtual void bind(EventPool& pool) = 0;
};

/**
 * @brief Centralized manager for controller registration and ownership.
 * 
 * ControllerManager owns all active controllers and ensures each controller type
 * is registered exactly once. Controllers are identified by RTTI type_index to
 * prevent duplicate registration.
 * 
 * Registration Process:
 * 1. Template instantiation via RegisterController<ControllerType>()
 * 2. RTTI lookup to check if type already exists
 * 3. Controller constructed via new and wrapped in unique_ptr
 * 4. bind() called to establish EventPool subscriptions
 * 5. Controller stored in type-indexed map
 * 
 * Ownership:
 * - Manager owns controllers via unique_ptr (exclusive ownership)
 * - Controllers are destroyed when manager is destroyed
 * - No external references to controller instances
 * 
 * @note Controllers are registered during Editor initialization phase.
 * @note Registration order does not matter (event-driven coordination).
 */
class ControllerManager 
{
private:
	std::unordered_map<std::type_index, std::unique_ptr<Controllers>> controller_list; ///< Type-indexed controller storage
public:
	/**
	 * @brief Register a controller of specified type and bind to EventPool.
	 * 
	 * Template method that instantiates a controller, binds it to the EventPool,
	 * and stores it in the manager. If a controller of the same type already exists,
	 * the call is silently ignored (idempotent registration).
	 * 
	 * Type Safety:
	 * - _Ctrler must inherit from Controllers
	 * - RTTI ensures unique registration per concrete type
	 * - Template allows compile-time type checking
	 * 
	 * @tparam _Ctrler Controller type to register (must inherit from Controllers)
	 * @param pool EventPool for the controller to subscribe to
	 * 
	 * @note Registration is idempotent - duplicate calls are no-ops.
	 * @note Controllers are immediately bound after construction.
	 * 
	 * Example:
	 * @code
	 * ControllerManager manager;
	 * EventPool events;
	 * manager.RegisterController<CameraController>(events);
	 * manager.RegisterController<ViewportController>(events);
	 * @endcode
	 */
	template<typename _Ctrler>
	void RegisterController(EventPool& pool);
};

template<typename _Ctrler>
inline void ControllerManager::RegisterController(EventPool& pool)
{
	std::type_index type = typeid(_Ctrler);
	if (controller_list.find(type) != controller_list.end())
		return;
	std::unique_ptr<Controllers> ctrler_ptr(new _Ctrler());
	ctrler_ptr->bind(pool);
	controller_list[type] = std::move(ctrler_ptr);
}
