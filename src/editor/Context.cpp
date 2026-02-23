#include "Context.h"
#include "SceneManager.h"

#include "events/EditorEvents.h"
#include "events/CameraEvents.h"

#include "macros.h"

const ObjectID* SceneContext::GetActiveCamera() const {
	Scene* scene = dynamic_cast<Scene*>(active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveCamera();
}

const ObjectID* SceneContext::GetActiveEnvironment() const {
	Scene* scene = dynamic_cast<Scene*>(active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetActiveEnvironment();
}

const ObjectID* SceneContext::GetPPS(int _tar) const {
	Scene* scene = dynamic_cast<Scene*>(active_scene);
	if (scene == nullptr) {
		return nullptr;
	}
	return scene->GetPPS(_tar);
}

const std::vector<const ObjectID*> SceneContext::GetObjectIDs() const
{
	Scene* scene = dynamic_cast<Scene*>(active_scene);
	if (scene == nullptr) {
		return {};
	}
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

		Scene* active_scene = dynamic_cast<Scene*>(scene.active_scene);
		if (active_scene == nullptr) return;

		ObjectID* obj = active_scene->GetObjectID(e.UID);
		if (obj == selected_obj) return;

		editor.selections.Select(active_scene->GetObjectID(e.UID), e.increment);
		pool.emit(SelectionChangedEvent{ obj });
		});

	pool.subscribe<RenderSurfaceResizedEvent>([this, &pool](RenderSurfaceResizedEvent e) {
		Camera* cam = (Camera*)scene.GetActiveCamera();
		if (cam == nullptr) return;

		pool.emit(CameraResizeEvent{ cam, e.width, e.height });

		Scene* active_scene = dynamic_cast<Scene*>(scene.active_scene);
		if (active_scene == nullptr) return;

		active_scene->UpdateSceneStatus(Scene::SceneChanged, true);
		});

	pool.subscribe<RenderConfigChangedEvent>([this, &pool](RenderConfigChangedEvent e) {
		Scene* active_scene = dynamic_cast<Scene*>(scene.active_scene);
		if (active_scene == nullptr) return;

		active_scene->SetSceneStatus(Scene::LightChanged, true);
		for (auto& [id, light] : active_scene->light_list) {
			// Shadow map format change is handled by ShadowSystem::Update internally.
			light->is_light_changed = true;
		}
		});
}
