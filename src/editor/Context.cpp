#include "Context.h"
#include "SceneManager.h"

#include "events/EditorEvents.h"
#include "events/CameraEvents.h"

#include "macros.h"

const ObjectID* SceneContext::GetActiveCamera() const {
	SceneResource* scene = dynamic_cast<SceneResource*>(active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveCamera();
}

const ObjectID* SceneContext::GetActiveEnvironment() const {
	SceneResource* scene = dynamic_cast<SceneResource*>(active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveEnvironment();
}

const ObjectID* SceneContext::GetPPS(int _tar) const {
	SceneResource* scene = dynamic_cast<SceneResource*>(active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetPPS(_tar);
}

const std::vector<const ObjectID*> SceneContext::GetObjectIDs() const
{
	SceneResource* scene = dynamic_cast<SceneResource*>(active_scene);
	std::vector<const ObjectID*> obj_list{};
	obj_list.reserve(scene->obj_list.size());
	for(const auto& [id, obj] : scene->obj_list) {
		obj_list.push_back(obj.get());
	}
	return obj_list;
}

void Context::Init(EventPool& pool)
{
	pool.subscribe<ObjectSelectedEvent>([this, &pool](const ObjectSelectedEvent& e) {
		const ObjectID* selected_obj = editor.selections.GetSelectedObjects();

		SceneResource* active_scene = dynamic_cast<SceneResource*>(scene.active_scene);
		ObjectID* obj = active_scene->GetObjectID(e.UID);
		if (obj == selected_obj) return;

		editor.selections.Select(active_scene->GetObjectID(e.UID), e.increament);
		pool.emit(SelectionChangedEvent{ obj });
		});

	pool.subscribe<RenderSurfaceResizedEvent>([this, &pool](RenderSurfaceResizedEvent e) {
		Camera* cam = (Camera*)scene.GetActiveCamera();
		pool.emit(CameraResizeEvent{ cam, e.width, e.height });

		SceneResource* active_scene = dynamic_cast<SceneResource*>(scene.active_scene);
		active_scene->UpdateSceneStatus(SceneResource::SceneChanged, true);
		});

	pool.subscribe<RenderConfigChangedEvent>([this, &pool](RenderConfigChangedEvent e) {
		SceneResource* active_scene = dynamic_cast<SceneResource*>(scene.active_scene);

		active_scene->SetSceneStatus(SceneResource::LightChanged, true);
		for (auto& [id, light] : active_scene->light_list) {
			light->InitShadowMap(e.config->RequiresMomentShadow());
			light->is_light_changed = true;
		}
		});
}
