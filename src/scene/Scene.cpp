#include "Scene.h"

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::UpdateSceneStatus(int tar, bool value)
{
	status = (SceneModifStatus)(status | (tar * value));
}

void Scene::SetSceneStatus(int tar, bool value)
{
	status = (SceneModifStatus)(status & (~tar));
	UpdateSceneStatus(tar, value);
}

bool Scene::CheckStatus(SceneModifStatus tar)
{
	return status & tar;
}

#include <stdlib.h>
void Scene::_debugStatus()
{
#ifdef _DEBUG
	char res[16];
	_itoa_s(status, res, 2);
	printf(res);
	printf("\n");
#endif // _DEBUG
}

void Scene::UseCamera(Resource<Camera> camera)
{
	if (cam_list.find(camera->GetObjectID()) == cam_list.end())
	{
		cam_list[0] = camera;
		return;
	}

	cam_list[camera->GetObjectID()] = camera;
	obj_list[camera->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(camera);
	cam_list[0] = camera;

	status = SceneModifStatus::SceneChanged;
}

void Scene::UseMesh(Resource<Mesh> mesh)
{
	if (mesh_list.find(mesh->GetObjectID()) != mesh_list.end())
		return;

	mesh_list[mesh->GetObjectID()] = mesh;
	obj_list[mesh->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(mesh);

	status = SceneModifStatus::SceneChanged;
}

void Scene::UseLight(Resource<Light> light)
{
	if (light_list.find(light->GetObjectID()) != light_list.end())
		return;

	light_list[light->GetObjectID()] = light;
	obj_list[light->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(light);

	sprite_list[light->light_sprite.GetObjectID()] = std::shared_ptr<Sprite>(light, &light->light_sprite);

	status = SceneModifStatus::SceneChanged;
}

void Scene::UsePolygonLight(Resource<PolygonLight> polyLight)
{
	if (poly_light_list.find(polyLight->GetObjectID()) != poly_light_list.end())
		return;

	poly_light_list[polyLight->GetObjectID()] = polyLight;
	obj_list[polyLight->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(polyLight);

	status = SceneModifStatus::SceneChanged;
}

void Scene::UseEnvironment(Resource<Environment> envir)
{
	if (envir_list.find(envir->GetObjectID()) != envir_list.end())
	{
		envir_list[0] = envir;
		return;
	}

	envir_list[envir->GetObjectID()] = envir;
	envir_list[0] = envir;
	obj_list[envir->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(envir);

	sprite_list[envir->envir_sprite.GetObjectID()] = std::shared_ptr<Sprite>(envir, &envir->envir_sprite);

	status = SceneModifStatus::SceneChanged;
}

void Scene::UseDebugLine(Resource<DebugLine> dline)
{
	if (dLine_list.find(dline->GetObjectID()) != dLine_list.end())
		return;

	dLine_list[dline->GetObjectID()] = dline;
	obj_list[dline->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(dline);

	status = SceneModifStatus::SceneChanged;
}

void Scene::UseDebugPoints(Resource<DebugPoints> dpoints)
{
	if (dPoints_list.find(dpoints->GetObjectID()) != dPoints_list.end())
		return;

	dPoints_list[dpoints->GetObjectID()] = dpoints;
	obj_list[dpoints->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(dpoints);

	status = SceneModifStatus::SceneChanged;
}

void Scene::UsePostProcessing(Resource<PostProcessing> pps)
{
	pps_list.emplace_back(pps);
	obj_list[pps->GetObjectID()] = std::dynamic_pointer_cast<ObjectID>(pps);
}


void Scene::UseSDF(Resource<SDFField> sdf)
{
	sdf_field = sdf;
	sdf_field->ResetBuffer();
	UpdateSceneStatus(SDFChanged, true);
}


ObjectID* Scene::GetObjectID(int _id)
{
	if (obj_list.find(_id) != obj_list.end())
		return obj_list[_id].get();
	else
		return nullptr;
}

Camera* Scene::GetActiveCamera()
{
	if (cam_list.find(0) != cam_list.end())
		return cam_list[0].get();
	else
		return nullptr;
}

Environment* Scene::GetActiveEnvironment()
{
	if (envir_list.find(0) != envir_list.end())
		return envir_list[0].get();
	else
		return nullptr;
}

PostProcessing* Scene::GetPPS(int _tar)
{
	assert(_tar >= 0 && _tar < pps_list.size());
	return pps_list[_tar].get();
}

void Scene::UpdateObjTransforms()
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
		UpdateSceneStatus(ShaderChanged, mesh->o_shader->is_shader_changed);
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
		UpdateSceneStatus(ShaderChanged, polyLight->o_shader.is_shader_changed);
	}

	for (auto& [id, dLine] : dLine_list)
	{
		dLine->ApplyAllTransform();
		UpdateSceneStatus(ObjectTransChanged, dLine->is_Uniform_changed);
		UpdateSceneStatus(ShaderChanged, dLine->dLine_shader.is_shader_changed);
	}

	for (auto& [id, dPoint] : dPoints_list)
	{
		dPoint->ApplyAllTransform();
		UpdateSceneStatus(ObjectTransChanged, dPoint->is_Uniform_changed);
		UpdateSceneStatus(ShaderChanged, dPoint->dp_shader[dPoint->dp_type].is_shader_changed);
	}

	for (auto& pps : pps_list)
	{
		UpdateSceneStatus(ShaderChanged, pps->pps_shader->is_shader_changed);
	}
}


void Scene::ResetStatus()
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
		polyLight->o_shader.is_shader_changed = false;
	}

	for (auto& [id, dLine] : dLine_list)
	{
		dLine->is_Uniform_changed = false;
		dLine->dLine_shader.is_shader_changed = false;
	}

	for (auto& [id, dPoint] : dPoints_list)
	{
		dPoint->is_Uniform_changed = false;
		dPoint->dp_shader[dPoint->dp_type].is_shader_changed = false;
	}

	for (auto& pps : pps_list)
	{
		pps->pps_shader->is_shader_changed = false;
	}

	GetActiveCamera()->is_Uniform_changed = false;
	GetActiveCamera()->is_invUniform_changed = false;
	GetActiveCamera()->is_frustum_changed = false;
}
