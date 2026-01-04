#pragma once

#include "ID.h"
#include "SelectionManager.h"

class SceneContext
{
public:
	// TODO: temprary solution
	mutable SelectionManager<ObjectID> c_selections;
	UID* c_active_scene;

	// TODO: use UID*
	void* c_active_fb_result;
	void* c_active_fb_channel;

	void UseScene(UID* scene) { c_active_scene = scene; };
	void UseResultFB(void* fb) { c_active_fb_result = fb; };
	void UseChannelFB(void* fb) { c_active_fb_channel = fb; };

	/* Read only */
	const ObjectID* GetActiveCamera() const;
	const ObjectID* GetActiveEnvironment() const;
	const ObjectID* GetPPS(int _tar) const;

	const std::vector<const ObjectID*> GetObjectIDs() const;
};

