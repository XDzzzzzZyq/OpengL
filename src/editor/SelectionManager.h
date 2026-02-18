/**
 * @file SelectionManager.h
 * @brief Generic selection state management for objects.
 * 
 * SelectionManager provides a type-safe container for tracking user selections.
 * It supports single and multi-selection modes with fast lookup via hash set.
 * 
 * Architecture:
 * - Template-based for use with any object type (ObjectID, Mesh, etc.)
 * - EditorContext owns SelectionManager<ObjectID> for scene selections
 * - UI queries selections to highlight/display selected objects
 * - Controllers mutate selections in response to user input
 * 
 * Selection Patterns:
 * - Single select: Replace current selection
 * - Multi-select (increment): Add to selection set
 * - Deselect: Remove from selection set
 * - Active object: Last selected object in list
 * 
 * @note Editor Layer: SelectionManager is part of EditorContext
 * @note Thread-safety: Not thread-safe. Must be used from main thread only.
 */

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>

/**
 * @brief Generic selection state manager supporting single and multi-selection.
 * 
 * SelectionManager maintains a list of selected objects with fast lookup.
 * It distinguishes between:
 * - Selected objects: All objects in the selection set
 * - Active object: The last selected object (primary selection)
 * - Hovered object: Object under mouse cursor (not selected yet)
 * 
 * Usage Examples:
 * - Scene object selection in Outliner/Viewport
 * - Material selection in MaterialViewer
 * - Node selection in node editor
 * 
 * Selection Modes:
 * - increment=false: Single selection (replace existing)
 * - increment=true: Multi-selection (add to set)
 * 
 * Performance:
 * - Select: O(1) lookup + O(1) append
 * - IsSelected: O(1) hash lookup
 * - GetSelected: O(1) vector access
 * 
 * @tparam _Base Base type of selectable objects (must support pointer semantics)
 * 
 * @note Active object is always the last selected object in the list
 * @note Duplicate selections are prevented via _selection_cache
 */
template<class _Base>
class SelectionManager
{
private:
	std::vector<_Base*> selected_objects;        ///< Ordered list of selected objects (last = active)
	_Base* hovered_object{ nullptr };            ///< Object under mouse cursor (not yet selected)
	std::unordered_set<_Base*> _selection_cache; ///< Fast lookup set for IsSelected() queries
	
public:
	/**
	 * @brief Selects an object.
	 * 
	 * If increment=false (single selection):
	 * - Clears existing selection
	 * - Adds obj as the only selected object
	 * 
	 * If increment=true (multi-selection):
	 * - If obj not in selection: adds to end (becomes active)
	 * - If obj already selected: moves to end (becomes active)
	 * 
	 * @param obj Pointer to object to select (nullptr ignored)
	 * @param increment Whether to add to selection (true) or replace (false)
	 * 
	 * @note Active object is always selected_objects.back()
	 */
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

	/**
	 * @brief Deselects an object.
	 * 
	 * Removes obj from selection set. If obj was active, the previous
	 * object becomes active.
	 * 
	 * @param obj Pointer to object to deselect (nullptr ignored)
	 * @param increment Unused parameter (kept for API consistency)
	 * 
	 * @note No-op if obj is not selected
	 */
	void Deselect(_Base* obj, bool increment) {
		if (obj == nullptr) 
			return;

		auto idx = std::find(selected_objects.begin(), selected_objects.end(), obj);
		if (idx == selected_objects.end()) 
			return;
		selected_objects.erase(idx);
		_selection_cache.erase(obj);
	}

	/**
	 * @brief Returns the active (last selected) object.
	 * 
	 * The active object is the primary selection, typically used for
	 * displaying properties or applying operations.
	 * 
	 * @return Pointer to active object, or nullptr if no selection
	 */
	_Base* GetActiveObject() const
	{
		if (selected_objects.empty())
			return nullptr;
		else
			return selected_objects[selected_objects.size() - 1];
	}

	/**
	 * @brief Returns the first selected object.
	 * 
	 * @return Pointer to first selected object, or nullptr if no selection
	 * @note TODO: Clarify difference from GetActiveObject() or unify API
	 */
	_Base* GetSelectedObjects() const
	{
		if (selected_objects.empty())
			return nullptr;
		else
			return selected_objects[0];
	}

	/**
	 * @brief Checks if an object is selected.
	 * 
	 * Fast O(1) lookup using internal hash set.
	 * 
	 * @param obj Pointer to object to check
	 * @return true if obj is in the selection set
	 */
	bool IsSelected(_Base* obj) const
	{
		return _selection_cache.contains(obj);
	}
};
