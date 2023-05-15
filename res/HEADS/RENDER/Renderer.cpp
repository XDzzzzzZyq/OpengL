#include "Renderer.h"

Renderer::Renderer()
{

}

void Renderer::Init()
{
	if (glewInit() != GLEW_OK)
		std::cout << "glew error" << std::endl;
	else
		std::cout << "glew has no error" << std::endl;

	if (glGetError() != GL_NO_ERROR)
		std::cout << "OpenGL Error: " << glGetError() << std::endl;
	else
		std::cout << "OpenGL has no error " << std::endl;


	DEBUG("Renderer Open")
		glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_MULTISAMPLE);

	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, 0xff);
	//glStencilMask(0xff);

	//glEnable(GL_CONVOLUTION_2D);

	r_render_result = FrameBuffer({ COMBINE_FB });
	AddFrameBuffer();


	EventInit();

	/*
	GLint maxAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);

	GLint maxDrawBuf = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);

	DEBUG(maxAttach)
		DEBUG(maxDrawBuf)*/
}

Renderer::~Renderer()
{
	for (auto& buffer : r_buffer_list)
		buffer.Del();
}

std::string Renderer::GetObjectName(int ID)
{
	return name_buff[ID];
}

int Renderer::GetSelectID(GLuint x, GLuint y)
{
	if (viewport_offset - ImVec2(5, 5) < ImVec2(x, y) && ImVec2(x, y) < viewport_offset + r_buffer_list[_RASTER].GetFrameBufferSize() * ImVec2(1, 2))
		//return GetActiveEnvironment()->envir_frameBuffer->ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
		return r_buffer_list[_RASTER].ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
	else
		return active_GO_ID;
}

void Renderer::AddFrameBuffer()
{
	r_buffer_list.emplace_back(std::vector<FBType>AVAIL_PASSES);
}

void Renderer::BindFrameBuffer(int slot)
{
	r_buffer_list[slot].BindFrameBuffer();
}

void Renderer::EndFrameBuffer(int slot)
{
	r_buffer_list[slot].UnbindFrameBuffer();
}

void Renderer::FrameBufferResize(int slot, const ImVec2& size)
{
	//GetActiveEnvironment()->envir_frameBuffer->Resize(size);
	r_buffer_list[_RASTER].Resize(size);
	r_render_result->Resize(size);
}

GLuint Renderer::GetFrameBufferTexture(int slot)
{
	//return framebuffer_list[slot].BufferTexture.GetTexID();
	//return r_buffer_list[_RASTER].GetFBTextureID(RAND_FB);
	return r_render_result->GetFBTextureID(COMBINE_FB);
}


void Renderer::EventInit()
{
	EventList[GenIntEvent(0, 0, 0, 1, 0)] = REGIST_EVENT(Renderer::LMB_CLICK);
	EventList[GenIntEvent(1, 0, 0, 0, 0)] = REGIST_EVENT(Renderer::SHIFT);

	EventListener::GetActiveShader = [&](int id) { return obj_list[id]->GetShaderStruct(); };
}

void Renderer::LMB_CLICK()
{
	if (!IsMouseClick()) return;
	pre_act_go_ID = active_GO_ID;

	active_GO_ID = GetSelectID(mouse_x, mouse_y);

	if (name_buff.find(active_GO_ID) != name_buff.end()) {
		if (pre_act_go_ID != 0 && obj_list.find(pre_act_go_ID) != obj_list.end())
			obj_list[pre_act_go_ID]->is_selected = false;

		if (obj_list.find(active_GO_ID) != obj_list.end()) {
			obj_list[active_GO_ID]->is_selected = true;
			active_shader = obj_list[active_GO_ID]->GetShaderStruct();
		}

	}
	else {
		active_GO_ID = 0;
		active_shader = nullptr;
		if (pre_act_go_ID != 0 && obj_list.find(pre_act_go_ID) != obj_list.end())
			obj_list[pre_act_go_ID]->is_selected = false;
	}

	if (multi_select) {
		is_selected_changed = pre_act_go_ID != active_GO_ID;
	}
	else {
		is_selected_changed = pre_act_go_ID != active_GO_ID;
	}

	for (auto i : spirit_id_buff) {
		if (i == active_GO_ID) {
			is_spirit_selected = true;
			return;
		}
		else {
			is_spirit_selected = false;
		}

	}


}

void Renderer::SHIFT()
{
	multi_select = true;
}

//////////////////////////////////////////////

void Renderer::UpdateFrame()
{
	glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.07f, 0.13f, 0.17f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearDepth(-10.0f);


	if (is_outliner_selected) {
		if (pre_act_go_ID != 0 && obj_list.find(pre_act_go_ID) != obj_list.end())
			obj_list[pre_act_go_ID]->is_selected = false;

		if (obj_list.find(active_GO_ID) != obj_list.end()) {
			obj_list[active_GO_ID]->is_selected = true;
			//active_shader = obj_list[active_GO_ID]->GetShaderStruct();
		}

		is_outliner_selected = false;
	}

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
/////////////////////////////////////// START RENDERING //////////////////////////////////////////////

void Renderer::Render(bool rend, bool buff) {

	if (cam_list.find(0) == cam_list.end()) _ASSERT("NONE ACTIVE CAMERA");
	if (envir_list.find(0) == envir_list.end()) _ASSERT("NONE ACTIVE ENVIRONMENT");

	glDisable(GL_BLEND);
	//glEnable(GL_BLEND);

	if (buff) {
		//GetActiveEnvironment()->BindFrameBuffer();
		r_buffer_list[_RASTER].BindFrameBuffer();
	}

	UpdateFrame();
	if (rend) {
		//glEnable(GL_STENCIL_TEST);
		;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		////////////    MESHES    ////////////
		GetActiveCamera()->ApplyTransform();
		GetActiveCamera()->GetInvTransform();
		GetActiveCamera()->GenFloatData();

		//DEBUG(viewport_offset)
		GetActiveEnvironment()->RenderEnvironment(GetActiveCamera());
		glEnable(GL_DEPTH_TEST);

		GetActiveEnvironment()->BindEnvironTexture();
		for (const auto& mesh : mesh_list)
		{
			if (!mesh.second->is_viewport)continue;

			mesh.second->ApplyAllTransform();
			mesh.second->RenderObj(GetActiveCamera(), {});
			mesh.second->is_Uniform_changed = false;
			mesh.second->o_shader->is_shader_changed = false;
		}
		is_light_changed = false;

		//////////// DEBUG MESHES ////////////

		for (const auto& dLine : dLine_list)
		{
			if (!dLine.second->is_viewport)continue;
			dLine.second->ApplyAllTransform();
			dLine.second->RenderDdbugLine(GetActiveCamera());
			dLine.second->is_Uniform_changed = false;
		}

		for (const auto& dPoints : dPoints_list)
		{
			if (!dPoints.second->is_viewport)continue;
			dPoints.second->ApplyTransform();
			dPoints.second->RenderDebugPoint(GetActiveCamera());
			dPoints.second->is_Uniform_changed = false;
		}

		////////////    ICONS    ////////////

		glEnable(GL_BLEND);
		for (const auto& light : light_list)
		{
			if (!light.second->light_spirit.is_viewport)continue;
			light.second->RenderLightSpr(GetActiveCamera());
		}
		for (const auto& envir : envir_list) {
			if (!envir.second->envir_spirit.is_viewport)continue;
			envir.second->RenderEnvirSpr(GetActiveCamera());
		}
	}
	if (buff) {
		//GetActiveEnvironment()->UnbindFrameBuffer();
		r_buffer_list[_RASTER].UnbindFrameBuffer();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		r_render_result->BindFrameBuffer();
		//GetActiveEnvironment()->envir_frameBuffer->BindFrameBufferTex(AVAIL_PASSES);
		r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		
 		static ComputeShader outline("selection_outline");
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 0);
		if (active_GO_ID != 0) outline.RunComputeShader(r_render_result->GetSize() / 4);

		pps_list[_PBR_COMP_PPS]->SetShaderValue("gamma", r_gamma);
		pps_list[_PBR_COMP_PPS]->SetShaderValue("Cam_pos", GetActiveCamera()->o_position);
		if (is_light_changed) {
			pps_list[_PBR_COMP_PPS]->SetShaderValue("Cam_pos", GetActiveCamera()->o_position);
		}
		pps_list[_PBR_COMP_PPS]->RenderPPS();

		r_render_result->UnbindFrameBuffer();


		static ComputeShader fxaa("FXAA");
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 1);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 2);
		if(r_using_fxaa) fxaa.RunComputeShader(r_render_result->GetSize() / 4);

		static ComputeShader editing("Editing");
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
		editing.RunComputeShader(r_render_result->GetSize() / 4);
	}
	//DEBUG(is_spirit_selected)
	Reset();
}

/////////////////////////////////////// FINISH RENDERING /////////////////////////////////////////////
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

void Renderer::Reset()
{
	////////////  RESET  ///////////

	if (multi_select) {
		multi_select = false;
		if (selec_list.size())
			selec_list.clear();
	}
	GetActiveCamera()->is_invUniform_changed = false;
	GetActiveCamera()->is_frustum_changed = false;
}




void Renderer::UseCamera(Camera* camera)
{
	if (cam_list.find(camera->GetObjectID()) == cam_list.end())
	{
		is_GOlist_changed = true;
		cam_list[camera->GetObjectID()] = camera;
		obj_list[camera->GetObjectID()] = camera;
		cam_list[0] = camera;
		outline_list.push_back(OutlineElement(camera->o_type, camera->GetObjectID(), camera->o_name, 0));
		parent_index_list.push_back(-1);
		name_buff[camera->GetObjectID()] = camera->o_name;
	}
	else {
		cam_list[0] = camera;
	}
}

void Renderer::UseCamera(const int& cam_id)
{
	if (cam_list.find(cam_id) != cam_list.end())
	{
		is_GOlist_changed = true;
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
		is_GOlist_changed = true;
		mesh_list[mesh->GetObjectID()] = mesh;
		obj_list[mesh->GetObjectID()] = mesh;
		outline_list.push_back(OutlineElement(mesh->o_type, mesh->GetObjectID(), mesh->o_name, 0));
		parent_index_list.push_back(-1);
		name_buff[mesh->GetObjectID()] = mesh->o_name;
	}

}

//////////////////////////////////////////////
void Renderer::UseLight(Light* light)
{
	if (light_list.find(light->GetObjectID()) == light_list.end())
	{
		is_GOlist_changed = true;
		is_light_changed = true;
		light_list[light->GetObjectID()] = light;
		obj_list[light->light_spirit.GetObjectID()] = light;

		outline_list.push_back(OutlineElement(light->o_type, light->light_spirit.GetObjectID(), light->o_name, 0));
		parent_index_list.push_back(-1);

		spirit_list[light->light_spirit.GetObjectID()] = &light->light_spirit;
		name_buff[light->light_spirit.GetObjectID()] = light->o_name; //using spirit ID
		spirit_id_buff.push_back(light->light_spirit.GetObjectID());
	}

}

void Renderer::UseEnvironment(Environment* envir)
{
	if (envir_list.find(envir->GetObjectID()) == envir_list.end())
	{
		is_GOlist_changed = true;
		envir_list[envir->GetObjectID()] = envir;
		envir_list[0] = envir;
		name_buff[envir->envir_spirit.GetObjectID()] = envir->o_name;
		obj_list[envir->envir_spirit.GetObjectID()] = envir;

		outline_list.push_back(OutlineElement(envir->o_type, envir->envir_spirit.GetObjectID(), envir->o_name, 0));
		parent_index_list.push_back(-1);

		spirit_list[envir->envir_spirit.GetObjectID()] = &envir->envir_spirit;
		name_buff[envir->envir_spirit.GetObjectID()] = envir->o_name; //using spirit ID
		spirit_id_buff.push_back(envir->envir_spirit.GetObjectID());
	}
	else {
		envir_list[0] = envir;
	}
}

void Renderer::UseEnvironment(const int& envir_id)
{
	if (envir_list.find(envir_id) != envir_list.end())
	{
		is_GOlist_changed = true;
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
		is_GOlist_changed = true;
		dLine_list[dline->GetObjectID()] = dline;
		obj_list[dline->GetObjectID()] = dline;
		outline_list.push_back(OutlineElement(dline->o_type, dline->GetObjectID(), dline->o_name, 0));
		name_buff[dline->GetObjectID()] = dline->o_name;
		parent_index_list.push_back(-1);
	}
}

void Renderer::UseDebugPoints(DebugPoints* dpoints)
{
	if (dPoints_list.find(dpoints->GetObjectID()) == dPoints_list.end())
	{
		is_GOlist_changed = true;
		dPoints_list[dpoints->GetObjectID()] = dpoints;
		obj_list[dpoints->GetObjectID()] = dpoints;
		outline_list.push_back(OutlineElement(dpoints->o_type, dpoints->GetObjectID(), dpoints->o_name, 0));
		name_buff[dpoints->GetObjectID()] = dpoints->o_name;
		parent_index_list.push_back(-1);
	}
}

void Renderer::UsePostProcessing(PostProcessing* pps)
{
	pps_list.push_back(pps);
}
