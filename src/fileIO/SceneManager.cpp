#include "SceneManager.h"

SceneResource::SceneResource()
{

}

SceneResource::~SceneResource()
{

}

void SceneResource::UpdateSceneStatus(int tar, bool mask)
{
	status = (SceneModifStatus)(status | (tar * mask));
}

void SceneResource::SetSceneStatus(int tar, bool value)
{
	status = (SceneModifStatus)(status & (~tar));
	UpdateSceneStatus(tar, value);
}

bool SceneResource::CheckStatus(SceneModifStatus tar)
{
	return status & tar;
}

#include <stdlib.h>
void SceneResource::_debugStatus()
{
#ifdef _DEBUG
	char res[16];
	_itoa_s(status, res, 2);
	printf(res);
	printf("\n");
#endif // _DEBUG
}

void SceneResource::UseCamera(Resource<Camera> camera)
{
	if (cam_list.find(camera->GetObjectID()) == cam_list.end())
	{
		cam_list[0] = camera;
		return;
	}

	cam_list[camera->GetObjectID()] = camera;
	obj_list[camera->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(camera);
	cam_list[0] = camera;
	EventListener::outline_list.push_back(OutlineElement(camera->o_type, camera->GetObjectID(), camera->o_name, 0));
	EventListener::parent_index_list.emplace_back(-1);

	status = SceneModifStatus::SceneChanged;
}

void SceneResource::UseMesh(Resource<Mesh> mesh)
{
	if (mesh_list.find(mesh->GetObjectID()) != mesh_list.end())
		return;

	mesh_list[mesh->GetObjectID()] = mesh;
	obj_list[mesh->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(mesh);
	EventListener::outline_list.push_back(OutlineElement(mesh->o_type, mesh->GetObjectID(), mesh->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	status = SceneModifStatus::SceneChanged;
}

void SceneResource::UseLight(Resource<Light> light)
{
	if (light_list.find(light->GetObjectID()) != light_list.end())
		return;

	light_list[light->GetObjectID()] = light;
	obj_list[light->light_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(light);

	EventListener::outline_list.push_back(OutlineElement(light->o_type, light->light_sprite.GetObjectID(), light->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	sprite_list[light->light_sprite.GetObjectID()] = std::shared_ptr<Sprite>(light, &light->light_sprite);

	status = SceneModifStatus::SceneChanged;
}

void SceneResource::UsePolygonLight(Resource<PolygonLight> polyLight)
{
	if (poly_light_list.find(polyLight->GetObjectID()) != poly_light_list.end())
		return;

	poly_light_list[polyLight->GetObjectID()] = polyLight;
	obj_list[polyLight->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(polyLight);
	EventListener::outline_list.push_back(OutlineElement(polyLight->o_type, polyLight->GetObjectID(), polyLight->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	status = SceneModifStatus::SceneChanged;
}

void SceneResource::UseEnvironment(Resource<Environment> envir)
{
	if (envir_list.find(envir->GetObjectID()) != envir_list.end())
	{
		envir_list[0] = envir;
		return;
	}

	envir_list[envir->GetObjectID()] = envir;
	envir_list[0] = envir;
	obj_list[envir->envir_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(envir);

	EventListener::outline_list.push_back(OutlineElement(envir->o_type, envir->envir_sprite.GetObjectID(), envir->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	sprite_list[envir->envir_sprite.GetObjectID()] = std::shared_ptr<Sprite>(envir, &envir->envir_sprite);

	status = SceneModifStatus::SceneChanged;
}

void SceneResource::UseDebugLine(Resource<DebugLine> dline)
{
	if (dLine_list.find(dline->GetObjectID()) != dLine_list.end())
		return;

	dLine_list[dline->GetObjectID()] = dline;
	obj_list[dline->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(dline);
	EventListener::outline_list.push_back(OutlineElement(dline->o_type, dline->GetObjectID(), dline->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	status = SceneModifStatus::SceneChanged;

}

void SceneResource::UseDebugPoints(Resource<DebugPoints> dpoints)
{
	if (dPoints_list.find(dpoints->GetObjectID()) != dPoints_list.end())
		return;

	dPoints_list[dpoints->GetObjectID()] = dpoints;
	obj_list[dpoints->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(dpoints);
	EventListener::outline_list.push_back(OutlineElement(dpoints->o_type, dpoints->GetObjectID(), dpoints->o_name, 0));
	EventListener::parent_index_list.push_back(-1);

	status = SceneModifStatus::SceneChanged;
}

void SceneResource::UsePostProcessing(Resource<PostProcessing> pps)
{
	pps_list.emplace_back(pps);
	obj_list[pps->pps_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(pps);

	EventListener::outline_list.push_back(OutlineElement(pps->o_type, pps->pps_sprite.GetObjectID(), pps->o_name, 0));
	EventListener::parent_index_list.push_back(-1);
}


void SceneResource::UseSDF(Resource<SDFField> sdf)
{
	sdf_field = sdf;
	sdf_field->ResetBuffer();
	UpdateSceneStatus(SDFChanged, true);
}

std::shared_ptr<Camera> SceneResource::GetActiveCamera()
{
	assert(cam_list.find(0) != cam_list.end());
	return cam_list[0];
}

std::shared_ptr<Environment> SceneResource::GetActiveEnvironment()
{
	assert(envir_list.find(0) != envir_list.end());
	return envir_list[0];
}

std::shared_ptr<PostProcessing> SceneResource::GetPPS(int _tar)
{
	assert(_tar >= 0 && _tar < pps_list.size());
	return pps_list[_tar];
}

void SceneResource::UpdateObjTransforms()
{
	// The update of transform should ignore the visibility of objects

	GetActiveCamera()->ApplyTransform();
	GetActiveCamera()->GetInvTransform();
	GetActiveCamera()->GenFloatData();
	UpdateSceneStatus(ObjectTransChanged | CameraChanged, GetActiveCamera()->is_Uniform_changed);

	for (auto& [id, mesh] : mesh_list) 
	{
		mesh->ApplyAllTransform();
		UpdateSceneStatus(ObjectTransChanged, mesh->is_Uniform_changed);
		UpdateSceneStatus(ShaderChanged, mesh->is_Uniform_changed);
		UpdateSceneStatus(SDFChanged, mesh->is_Uniform_changed && mesh->using_sdf);
		UpdateSceneStatus(MaterialChanged, mesh->o_material->is_mat_changed);
	}

	for (auto& [id, light] : light_list) 
	{
		light->ApplyAllTransform();
		/*	    Capture Status		*/
		UpdateSceneStatus(LightChanged, light->is_Uniform_changed || light->is_light_changed);
		UpdateSceneStatus(ObjectTransChanged, light->is_Uniform_changed);
	}

	for (auto& [id, polyLight] : poly_light_list) 
	{
		polyLight->ApplyAllTransform();
		UpdateSceneStatus(ObjectTransChanged, polyLight->is_Uniform_changed);
	}

	for (auto& [id, dLine] : dLine_list)
	{
		dLine->ApplyAllTransform();
		UpdateSceneStatus(ObjectTransChanged, dLine->is_Uniform_changed);
	}

	for (auto& [id, dPoint] : dPoints_list)
	{
		dPoint->ApplyAllTransform();
		UpdateSceneStatus(ObjectTransChanged, dPoint->is_Uniform_changed);
	}

	for (auto& pps : pps_list)
	{
		UpdateSceneStatus(ShaderChanged, pps->pps_shader->is_shader_changed);
	}
}


void SceneResource::ResetStatus()
{
	const bool sdf_changed = status & SDFChanged;
	status = NoChanges;
	UpdateSceneStatus(SDFChanged, sdf_changed);

	for (auto& [id, light] : light_list) 
	{
		light->is_light_changed = false;
		light->is_Uniform_changed = false;
	}

	for (auto& [id, mesh] : mesh_list)
	{
		mesh->is_Uniform_changed = false;
		mesh->o_shader->is_shader_changed = false;
		mesh->o_material->is_mat_changed = false;
		mesh->o_material->is_mat_struct_changed = false;
	}

	for (auto& [id, polyLight] : poly_light_list)
	{
		polyLight->is_Uniform_changed = false;
		polyLight->o_shader->is_shader_changed = false;
	}

	for (auto& [id, dLine] : dLine_list)
	{
		dLine->is_Uniform_changed = false;
	}

	for (auto& [id, dPoint] : dPoints_list)
	{
		dPoint->is_Uniform_changed = false;
	}

	for (auto& pps : pps_list)
	{
		pps->pps_shader->is_shader_changed = false;
	}

	GetActiveCamera()->is_Uniform_changed = false;
	GetActiveCamera()->is_invUniform_changed = false;
	GetActiveCamera()->is_frustum_changed = false;
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
