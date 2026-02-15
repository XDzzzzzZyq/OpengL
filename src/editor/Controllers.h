#pragma once
#include "Events.h"

#include <unordered_map>
#include <typeindex>
#include <memory>

class Controllers
{
public:
	virtual void bind(EventPool& pool) = 0;
};

class ControllerManager 
{
private:
	std::unordered_map<std::type_index, std::unique_ptr<Controllers>> controller_list;
public:
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
