#pragma once

#include <string>
#include <vector>
#include <unordered_set>

template<class _Base>
struct SelectEvent {
	const _Base* _tar{ nullptr };
	bool increment{ false };
};

template<class _Base>
class SelectionManager
{
private:
	std::vector<_Base*> selected_objects;
	_Base* hovered_object{ nullptr };
	std::unordered_set<_Base*> _selection_cache;
public:
	void Select(_Base* obj, bool increment)
	{
		if (obj == nullptr) return;

		if (increment) {
			auto idx = std::find(selected_objects.begin(), selected_objects.end(), obj);
			if (idx == selected_objects.end()) {
				selected_objects.push_back(obj);
				_selection_cache.insert(obj);
			}
			else {
				std::swap(*idx, selected_objects[selected_objects.size() - 1]);
			}
		}
		else {
			selected_objects.clear();
			selected_objects.push_back(obj);
			_selection_cache.clear();
			_selection_cache.insert(obj);
		}
	}

	void Deselect(_Base* obj, bool increment) {
		if (obj == nullptr) 
			return;

		auto idx = std::find(selected_objects.begin(), selected_objects.end(), obj);
		if (idx == selected_objects.end()) 
			return;
		selected_objects.erase(idx);
		_selection_cache.erase(obj);
	}

	// The last selected element is the actived
	_Base* GetActiveObject() const
	{
		if (selected_objects.empty())
			return nullptr;
		else
			return selected_objects[selected_objects.size() - 1];
	}

	_Base* GetSelectedObjects() const
	{
		if (selected_objects.empty())
			return nullptr;
		else
			return selected_objects[0];
	}

	bool IsSelected(_Base* obj) const
	{
		return _selection_cache.contains(obj);
	}
};
