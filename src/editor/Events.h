/**
 * @file Events.h
 * @brief Type-safe event system for decoupled communication across layers.
 * 
 * The EventPool enables publish-subscribe pattern with compile-time type safety.
 * Components emit events without knowing subscribers, preventing direct coupling.
 * 
 * Architecture:
 * - UI emits user actions as events
 * - Editor subscribes to events and mutates state
 * - Renderer subscribes to config changes
 * - No layer directly calls into another layer
 */

#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>

/**
 * @brief Base class for event types (currently unused).
 * @note TODO: Add event ID and type metadata
 */
class Events
{
	// TODO: event id and type
};

/**
 * @brief Type-safe event dispatcher using template-based publish-subscribe.
 * 
 * EventPool allows any component to subscribe to events of a specific type and
 * emit events to all registered handlers. Type safety is enforced at compile time
 * via templates, while runtime dispatch uses std::type_index.
 * 
 * Usage:
 * @code
 * EventPool pool;
 * 
 * // Subscribe to an event type
 * pool.subscribe<MyEvent>([](const MyEvent& e) {
 *     // Handle event
 * });
 * 
 * // Emit an event
 * pool.emit(MyEvent{param1, param2});
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. All subscribe/emit must occur on main thread.
 * @note Event lifetime: Events are passed by const reference and must not be modified.
 */
class EventPool {
public:
	/**
	 * @brief Handler function type for events of type Event.
	 * @tparam Event The event type to handle
	 */
	template<typename Event>
	using Handler = std::function<void(const Event&)>;

	/**
	 * @brief Subscribes a handler to events of type Event.
	 * 
	 * Registers a callback to be invoked whenever an event of type Event is emitted.
	 * Multiple handlers can subscribe to the same event type and will be called
	 * in subscription order.
	 * 
	 * @tparam Event The event type to subscribe to
	 * @param handler Callback function receiving const Event&
	 * 
	 * @note Handlers should not assume execution order relative to other handlers.
	 * @note Handlers execute synchronously within emit() call.
	 */
	template<typename Event>
	void subscribe(Handler<Event> handler) {
		auto& vec = handlers[typeid(Event)];
		vec.push_back(
			[h = std::move(handler)](const void* e) {
				h(*static_cast<const Event*>(e));
			}
		);
	}

	/**
	 * @brief Emits an event to all subscribed handlers.
	 * 
	 * Invokes all handlers registered for the Event type. Handlers are called
	 * synchronously in subscription order. If no handlers are registered,
	 * this is a no-op.
	 * 
	 * @tparam Event The event type to emit
	 * @param event The event instance to pass to handlers (by const reference)
	 * 
	 * @note Events are immutable from handlers (const Event&).
	 * @note All handlers complete before emit() returns.
	 */
	template<typename Event>
	void emit(const Event& event) const {
		auto it = handlers.find(typeid(Event));
		if (it == handlers.end()) return;

		for (auto& fn : it->second) {
			fn(&event);
		}
	}

private:
	/**
	 * @brief Maps event types to handler lists.
	 * 
	 * Uses std::type_index for runtime type discrimination and type-erased
	 * void* for handler storage. Type safety is preserved via template wrappers.
	 */
	std::unordered_map<
		std::type_index,
		std::vector<std::function<void(const void*)>>
	> handlers;

public:
	/**
	 * @brief Emits a global event to all systems.
	 * @note TODO: Define global event semantics
	 */
	void EmitGlobalEvent();
};

