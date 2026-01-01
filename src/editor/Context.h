#pragma once

#include "GameObject.h"
#include "SelectionManager.h"

class SceneContext
{
public:
	// TODO: temprary solution
	mutable SelectionManager<GameObject> c_selections;
	UID* c_active_scene;

	// TODO: use UID*
	void* c_active_fb_result;
	void* c_active_fb_channel;

	void UseScene(UID* scene) { c_active_scene = scene; };
	void UseResultFB(void* fb) { c_active_fb_result = fb; };
	void UseChannelFB(void* fb) { c_active_fb_channel = fb; };

	/* Read only */
	const GameObject* GetActiveCamera() const;
	const GameObject* GetActiveEnvironment() const;
	const GameObject* GetPPS(int _tar) const;

	const std::vector<const GameObject*> GetGameObjects() const;
};

