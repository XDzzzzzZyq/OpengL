#pragma once

#include "ID.h"
#include "SelectionManager.h"
#include "Events.h"
#include "RenderConfigs.h"

class SceneContext
{
public:
	UID* active_scene{ nullptr };

	void UseScene(UID* scene) { active_scene = scene; };

	/* Read only */
	const ObjectID* GetActiveCamera() const;
	const ObjectID* GetActiveEnvironment() const;
	const ObjectID* GetPPS(int _tar) const;

	const std::vector<const ObjectID*> GetObjectIDs() const;
};

class EditorContext
{
public:
	SelectionManager<ObjectID> selections;
};

class RenderContext
{
public: // TODO: private
	RenderConfigs* config{ nullptr };
public:
	RenderConfigs* GetConfig() const { return config; }
};

class Context {
public:
	SceneContext scene{};
	EditorContext editor{};
	RenderContext render{};

public:
	void Init(EventPool& pool);
};

