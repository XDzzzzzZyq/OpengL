#include "SceneManager.h"

SceneResource::SceneResource()
{

}

SceneResource::~SceneResource()
{

}

void SceneResource::UseCamera(Resource<Camera> camera)
{
	if (cam_list.find(camera->GetObjectID()) == cam_list.end())
	{
		cam_list[0] = camera;
		return;
	}

	is_scene_changed = true;
	cam_list[camera->GetObjectID()] = camera;
	obj_list[camera->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(camera);
	cam_list[0] = camera;
	EventListener::outline_list.push_back(OutlineElement(camera->o_type, camera->GetObjectID(), camera->o_name, 0));
	EventListener::parent_index_list.emplace_back(-1);
}

void SceneResource::UseMesh(Resource<Mesh> mesh)
{
	if (mesh_list.find(mesh->GetObjectID()) != mesh_list.end())
		return;

	is_scene_changed = true;
	mesh_list[mesh->GetObjectID()] = mesh;
	obj_list[mesh->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(mesh);
	EventListener::outline_list.push_back(OutlineElement(mesh->o_type, mesh->GetObjectID(), mesh->o_name, 0));
	EventListener::parent_index_list.push_back(-1);
}

void SceneResource::UseLight(Resource<Light> light)
{
	if (light_list.find(light->GetObjectID()) != light_list.end())
		return;

	is_scene_changed = true;
	light_list[light->GetObjectID()] = light;
	obj_list[light->light_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(light);

	EventListener::outline_list.push_back(OutlineElement(light->o_type, light->light_sprite.GetObjectID(), light->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	sprite_list[light->light_sprite.GetObjectID()] = std::shared_ptr<Sprite>(light, &light->light_sprite);
}

void SceneResource::UsePolygonLight(Resource<PolygonLight> polyLight)
{
	if (poly_light_list.find(polyLight->GetObjectID()) != poly_light_list.end())
		return;

	is_scene_changed = true;
	poly_light_list[polyLight->GetObjectID()] = polyLight;
	obj_list[polyLight->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(polyLight);
	EventListener::outline_list.push_back(OutlineElement(polyLight->o_type, polyLight->GetObjectID(), polyLight->o_name, 0));
	EventListener::parent_index_list.push_back(-1);
}

void SceneResource::UseEnvironment(Resource<Environment> envir)
{
	if (envir_list.find(envir->GetObjectID()) != envir_list.end())
	{
		envir_list[0] = envir;
		return;
	}

	is_scene_changed = true;
	envir_list[envir->GetObjectID()] = envir;
	envir_list[0] = envir;
	obj_list[envir->envir_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(envir);

	EventListener::outline_list.push_back(OutlineElement(envir->o_type, envir->envir_sprite.GetObjectID(), envir->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	sprite_list[envir->envir_sprite.GetObjectID()] = std::shared_ptr<Sprite>(envir, &envir->envir_sprite);
}

void SceneResource::UseDebugLine(Resource<DebugLine> dline)
{
	if (dLine_list.find(dline->GetObjectID()) != dLine_list.end())
		return;

	is_scene_changed = true;
	dLine_list[dline->GetObjectID()] = dline;
	obj_list[dline->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(dline);
	EventListener::outline_list.push_back(OutlineElement(dline->o_type, dline->GetObjectID(), dline->o_name, 0));
	EventListener::parent_index_list.push_back(-1);
}

void SceneResource::UseDebugPoints(Resource<DebugPoints> dpoints)
{
	if (dPoints_list.find(dpoints->GetObjectID()) != dPoints_list.end())
		return;

	is_scene_changed = true;
	dPoints_list[dpoints->GetObjectID()] = dpoints;
	obj_list[dpoints->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(dpoints);
	EventListener::outline_list.push_back(OutlineElement(dpoints->o_type, dpoints->GetObjectID(), dpoints->o_name, 0));
	EventListener::parent_index_list.push_back(-1);
}

void SceneResource::UsePostProcessing(Resource<PostProcessing> pps)
{
	pps_list.emplace_back(pps);
	obj_list[pps->pps_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(pps);

	EventListener::outline_list.push_back(OutlineElement(pps->o_type, pps->pps_sprite.GetObjectID(), pps->o_name, 0));
	EventListener::parent_index_list.push_back(-1);
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, std::shared_ptr<SceneResource>> SceneManager::sce_configs;

std::shared_ptr<SceneResource> SceneManager::GetSceneConfig(std::string _name)
{
	if (SceneManager::sce_configs.find(_name) == SceneManager::sce_configs.end())
		return nullptr;

	return SceneManager::sce_configs[_name];
}
