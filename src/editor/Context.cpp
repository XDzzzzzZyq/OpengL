#include "Context.h"
#include "SceneManager.h"

#include "events/EditorEvents.h"
#include "events/CameraEvents.h"

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

void SceneContext::Init(EventPool& pool)
{
	pool.subscribe<ObjectSelectedEvent>([this, &pool](const ObjectSelectedEvent& e) {
		const ObjectID* selected_obj = c_selections.GetSelectedObjects();

		SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
		ObjectID* obj = scene->GetObjectID(e.UID);
		if (obj == selected_obj) return;

		c_selections.Select(scene->GetObjectID(e.UID), e.increament);
		pool.emit(SelectionChangedEvent{ obj });
		});

	pool.subscribe<RenderSurfaceResizedEvent>([this, &pool](RenderSurfaceResizedEvent e) {
		Camera* cam = (Camera*)GetActiveCamera();
		pool.emit(CameraResizeEvent{ cam, e.width, e.height });

		SceneResource* scene = dynamic_cast<SceneResource*>(c_active_scene);
		scene->UpdateSceneStatus(SceneResource::SceneChanged, true);
		});
}
