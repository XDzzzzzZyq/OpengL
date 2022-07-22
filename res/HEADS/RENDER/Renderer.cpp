#include "Renderer.h"

Renderer::Renderer()
{
	DEBUG("Renderer Open")
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
}

Renderer::~Renderer()
{

}
//////////////////////////////////////////////
void Renderer::UseCamera(Camera* camera)
{
	if (cam_list.find(camera->GetObjectID()) == cam_list.end())
	{
		cam_list[camera->GetObjectID()] = camera;
		cam_list[0] = camera;

	}
	else {
		cam_list[0] = camera;
	}
}

void Renderer::UseCamera(const int& cam_id)
{
	if (cam_list.find(cam_id) != cam_list.end())
	{
		cam_list[0] = cam_list[cam_id];
	}

}

Camera* Renderer::GetActiveCamera()
{
	return cam_list[0];
}

//////////////////////////////////////////////
void Renderer::UseMesh(Mesh* mesh)
{
	if (mesh_list.find(mesh->GetObjectID()) == mesh_list.end())
	{
		mesh_list[mesh->GetObjectID()] = mesh;
	}

}

//////////////////////////////////////////////
void Renderer::UseLight(Light* light)
{
	if (light_list.find(light->GetObjectID()) == light_list.end())
	{
		is_light_changed = true;
		light_list[light->GetObjectID()] = light;
		spirit_list[light->light_spirit.GetObjectID()] = &light->light_spirit;
	}

}

void Renderer::UseEnvironment(Environment* envir)
{
	if (envir_list.find(envir->GetObjectID()) == envir_list.end())
	{
		envir_list[envir->GetObjectID()] = envir;
		envir_list[0] = envir;

	}
	else {
		envir_list[0] = envir;
	}
}

void Renderer::UseEnvironment(const int& envir_id)
{
	if (envir_list.find(envir_id) != envir_list.end())
	{
		envir_list[0] = envir_list[envir_id];
	}
}

void Renderer::UseDebugLine(DebugLine* dline)
{
	if (dLine_list.find(dline->GetObjectID()) == dLine_list.end())
	{
		dLine_list[dline->GetObjectID()] = dline;
		
	}
}

void Renderer::FrameClean() const
{
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	//glClearDepth(-10.0f);
	glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::Render() {

	if (cam_list.find(0)==cam_list.end())_ASSERT("NONE ACTIVE CAMERA");

	////////////    MESHES    ////////////
	cam_list[0]->ApplyTransform();
	cam_list[0]->GenFloatData();
	if (is_light_changed)
	{
		for (const auto& obj : mesh_list)
		{

			if (obj.second->is_rendered)
			{
				obj.second->ApplyTransform();
				obj.second->RenderObj(*cam_list[0], light_list);
				//std::cout << cam_list[0]->o_InvTransform;
			}
		}
		is_light_changed = false;
	}
	else
	{
		for (const auto& obj : mesh_list)
		{

			if (obj.second->is_rendered)
			{
				obj.second->ApplyTransform();
				obj.second->RenderObj(*cam_list[0], emptyLight);


			}
		}
	}
	

	//////////// DEBUG MESHES ////////////

	for (const auto& dLine : dLine_list)
	{
		dLine.second->RenderDline(cam_list[0]->o_InvTransform, cam_list[0]->cam_frustum);
	}

	////////////    IOCONS    ////////////

	for (const auto& light : light_list)
	{
		light.second->light_spirit.RenderSpirit(vec3_stdVec6(light.second->o_position,light.second->light_color) , cam_list[0]->o_InvTransform, cam_list[0]->cam_frustum);
	}


}

