#include "Renderer.h"

Renderer::Renderer()
{
	if (glewInit() != GLEW_OK) {
		std::cout << "glew error" << std::endl;
	}
	else {
		std::cout << "glew has no error" << std::endl;
	}

	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "OpenGL Error: " << glGetError() << std::endl;
	}
	else {
		std::cout << "OpenGL has no error " << std::endl;
	}


	DEBUG("Renderer Open")
		glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS,1,0xff);
	glStencilMask(0xff);


	framebuffer = FrameBuffer();
}

Renderer::~Renderer()
{

}

void Renderer::AddFrameBuffer()
{
	framebuffer_list.push_back(FrameBuffer());

	framebuffer_count++;
}

void Renderer::BindFrameBuffer(int slot)
{
	framebuffer_list[slot].BindFrameBuffer();
}

void Renderer::EndFrameBuffer(int slot)
{
	framebuffer_list[slot].UnbindFrameBuffer();
}

void Renderer::FrameBufferResize(int slot, const ImVec2& size)
{
	GetActiveEnvironment()->envir_frameBuffer->Resize(size);
	framebuffer->Resize(size);
}

GLuint Renderer::GetFrameBufferTexture(int slot)
{
	//return framebuffer_list[slot].BufferTexture.GetTexID();
	return framebuffer->BufferTexture.GetTexID();
}

//////////////////////////////////////////////

void Renderer::FrameClean() const
{
	//glClearColor(0.07f, 0.13f, 0.17f, 0.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearDepth(-10.0f);
	glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// START RENDERING ///////////////////////////////////////////////
void Renderer::Render() {

	if (cam_list.find(0) == cam_list.end())_ASSERT("NONE ACTIVE CAMERA");
	if (envir_list.find(0) == envir_list.end())_ASSERT("NONE ACTIVE ENVIRONMENT");

	//GLDEBUG
	
	envir_list[0]->BindFrameBuffer();


	FrameClean();
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	////////////    MESHES    ////////////
	cam_list[0]->ApplyTransform();
	cam_list[0]->GetInvTransform();
	cam_list[0]->GenFloatData();
	envir_list[0]->envir_hdr.Bind(HDR_TEXTURE);

	if (is_light_changed)
	{
		for (const auto& obj : mesh_list)
		{

			if (!obj.second->is_viewport)continue;

			obj.second->ApplyTransform();
			obj.second->RenderObj(cam_list[0], light_list);
			obj.second->is_Uniform_changed = false;
			//std::cout << cam_list[0]->o_InvTransform;
		}
		is_light_changed = false;
	}
	else
	{
		for (const auto& obj : mesh_list)
		{

			if (!obj.second->is_viewport)continue;

			obj.second->ApplyTransform();
			obj.second->RenderObj(cam_list[0], emptyLight);
			obj.second->is_Uniform_changed = false;
		}
	}

	envir_list[0]->envir_hdr.Unbind();
	glDisable(GL_STENCIL_TEST);
	//////////// DEBUG MESHES ////////////

	for (const auto& dLine : dLine_list)
	{
		if (!dLine.second->is_viewport)continue;
		dLine.second->ApplyTransform();
		dLine.second->RenderDdbugLine(cam_list[0]);
		dLine.second->is_Uniform_changed = false;
	}

	for (const auto& dPoints : dPoints_list)
	{
		if (!dPoints.second->is_viewport)continue;
		dPoints.second->ApplyTransform();
		dPoints.second->RenderDebugPoint(cam_list[0]);
		dPoints.second->is_Uniform_changed = false;
	}

	////////////    ICONS    ////////////

	for (const auto& light : light_list)
	{
		if (!light.second->light_spirit.is_viewport)continue;
		light.second->light_spirit.RenderSpirit(vec3_stdVec6(light.second->o_position, light.second->light_color), cam_list[0]);
	}

	envir_list[0]->UnBindFrameBuffer();
	//glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	//glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	framebuffer->BindFrameBuffer();
	envir_list[0]->RenderEnvironment(cam_list[0]);
	framebuffer->UnbindFrameBuffer();
	////////////  RESET  ///////////
	

	cam_list[0]->is_invUniform_changed = false;
	cam_list[0]->is_frustum_changed = false;
	is_scr_changed = false;
	
}


////////////////////////////////////// END RENDERING /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//++++++++                                                                                ++++++++++//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


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

Environment* Renderer::GetActiveEnvironment()
{
	return envir_list[0];
}

void Renderer::UseDebugLine(DebugLine* dline)
{
	if (dLine_list.find(dline->GetObjectID()) == dLine_list.end())
	{
		dLine_list[dline->GetObjectID()] = dline;

	}
}

void Renderer::UseDebugPoints(DebugPoints* dpoints)
{
	if (dPoints_list.find(dpoints->GetObjectID()) == dPoints_list.end())
	{
		dPoints_list[dpoints->GetObjectID()] = dpoints;

	}
}
