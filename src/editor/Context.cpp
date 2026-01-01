#include "Context.h"
#include "SceneManager.h"

const GameObject* SceneContext::GetActiveCamera() const {
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveCamera();
}

const GameObject* SceneContext::GetActiveEnvironment() const {
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveEnvironment();
}

const GameObject* SceneContext::GetPPS(int _tar) const {
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetPPS(_tar);
}

const std::vector<const GameObject*> SceneContext::GetGameObjects() const
{
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	std::vector<const GameObject*> obj_list{};
	obj_list.reserve(scene->obj_list.size());
	for(const auto& [id, obj] : scene->obj_list) {
		obj_list.push_back(obj.get());
	}
	return obj_list;
}
