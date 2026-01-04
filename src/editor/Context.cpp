#include "Context.h"
#include "SceneManager.h"

const ObjectID* SceneContext::GetActiveCamera() const {
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveCamera();
}

const ObjectID* SceneContext::GetActiveEnvironment() const {
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveEnvironment();
}

const ObjectID* SceneContext::GetPPS(int _tar) const {
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetPPS(_tar);
}

const std::vector<const ObjectID*> SceneContext::GetObjectIDs() const
{
	SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
	std::vector<const ObjectID*> obj_list{};
	obj_list.reserve(scene->obj_list.size());
	for(const auto& [id, obj] : scene->obj_list) {
		obj_list.push_back(obj.get());
	}
	return obj_list;
}
