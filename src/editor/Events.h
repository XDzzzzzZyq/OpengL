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
 * 
 * Execution model:
 * - emit() enqueues an event for deferred dispatch.
 * - process() drains the queue, dispatching all pending events in FIFO order.
 * - Events emitted from within a handler are appended to the end of the queue,
 *   ensuring deterministic breadth-first ordering for event chains.
 * - Call process() once per frame, after the UI layer has finished rendering.
 */

#pragma once

#include <functional>
#include <queue>
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
 * @brief Type-safe event dispatcher with deferred execution queue.
 * 
 * EventPool allows any component to subscribe to events of a specific type and
 * emit events to be dispatched in a controlled, deferred manner. Type safety is
 * enforced at compile time via templates, while runtime dispatch uses std::type_index.
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
 * // Enqueue an event (not dispatched yet)
 * pool.emit(MyEvent{param1, param2});
 * 
 * // Dispatch all queued events (call once per frame, after UI rendering)
 * pool.process();
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. All subscribe/emit/process must occur on main thread.
 * @note Event chains: If a handler calls emit(), the new event is appended to the end of
 *       the queue and processed in the same process() call (breadth-first ordering).
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
	 * Registers a callback to be invoked whenever an event of type Event is
	 * dispatched via process(). Multiple handlers can subscribe to the same
	 * event type and will be called in subscription order.
	 * 
	 * @tparam Event The event type to subscribe to
	 * @param handler Callback function receiving const Event&
	 * 
	 * @note Handlers should not assume execution order relative to other handlers.
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
	 * @brief Enqueues an event for deferred dispatch.
	 * 
	 * The event is copied and stored in the internal queue. It will not be
	 * dispatched until process() is called. If called from within a handler
	 * during process(), the new event is appended to the end of the queue
	 * and processed in the same process() call.
	 * 
	 * @tparam Event The event type to emit
	 * @param event The event instance to enqueue (copied into the queue)
	 */
	template<typename Event>
	void emit(const Event& event) {
		event_queue.push([this, ev = event]() {
			dispatch(ev);
		});
	}

	/**
	 * @brief Dispatches all enqueued events in FIFO order.
	 * 
	 * Processes every event currently in the queue. Events emitted by handlers
	 * during processing are appended to the end of the queue and handled within
	 * the same call, guaranteeing deterministic breadth-first ordering for
	 * event chains.
	 * 
	 * @note Call once per frame, after the UI layer has finished rendering.
	 */
	void process() {
		while (!event_queue.empty()) {
			auto fn = std::move(event_queue.front());
			event_queue.pop();
			fn();
		}
	}

private:
	/**
	 * @brief Immediately dispatches an event to all registered handlers.
	 * 
	 * @tparam Event The event type to dispatch
	 * @param event The event instance to pass to handlers
	 */
	template<typename Event>
	void dispatch(const Event& event) {
		auto it = handlers.find(typeid(Event));
		if (it == handlers.end()) return;

		for (auto& fn : it->second) {
			fn(&event);
		}
	}

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

	/// @brief Queue of pending event dispatch closures.
	std::queue<std::function<void()>> event_queue;

public:
	/**
	 * @brief Emits a global event to all systems.
	 * @note TODO: Define global event semantics
	 */
	void EmitGlobalEvent();
};

