#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>

class Events
{
	// TODO: event id and type
};

class EventPool {
public:
	template<typename Event>
	using Handler = std::function<void(const Event&)>;

	template<typename Event>
	void subscribe(Handler<Event> handler) {
		auto& vec = handlers[typeid(Event)];
		vec.push_back(
			[h = std::move(handler)](const void* e) {
				h(*static_cast<const Event*>(e));
			}
		);
	}

	template<typename Event>
	void emit(const Event& event) {
		auto it = handlers.find(typeid(Event));
		if (it == handlers.end()) return;

		for (auto& fn : it->second) {
			fn(&event);
		}
	}

private:
	std::unordered_map<
		std::type_index,
		std::vector<std::function<void(const void*)>>
	> handlers;

public:
	void EmitGlobalEvent();
};

