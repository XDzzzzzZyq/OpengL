#pragma once

#include "ID.h"
#include "SelectionManager.h"
#include "Events.h"

class SceneContext
{
public:
	// TODO: temprary solution
	mutable SelectionManager<ObjectID> c_selections;
	UID* c_active_scene;

	void UseScene(UID* scene) { c_active_scene = scene; };

	/* Read only */
	const ObjectID* GetActiveCamera() const;
	const ObjectID* GetActiveEnvironment() const;
	const ObjectID* GetPPS(int _tar) const;

	const std::vector<const ObjectID*> GetObjectIDs() const;

public:
	void Init(EventPool& pool);
};

