#include "Renderer.h"

Renderer::Renderer()
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

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilMask(0xff);

	glEnable(GL_CONVOLUTION_2D);


	framebuffer = FrameBuffer(COMBINE_FB);

	EventInit();
}

Renderer::~Renderer()
{

}

std::string Renderer::GetObjectName(int ID)
{
	return name_buff[ID];
}

int Renderer::GetSelectID(GLuint x, GLuint y)
{
	if (viewport_offset < ImVec2(x, y) && ImVec2(x, y) < viewport_offset+GetActiveEnvironment()->envir_frameBuffer->GetFrameBufferSize())
		return GetActiveEnvironment()->envir_frameBuffer->ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
	else
		return active_GO_ID;
}

void Renderer::AddFrameBuffer()
{
	framebuffer_list.push_back(FrameBuffer(COMBINE_FB));

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


void Renderer::EventInit()
{
	EventList[GenIntEvent(0, 0, 0, 1, 0)] = std::bind(&Renderer::LMB_CLICK, this);
	EventList[GenIntEvent(1, 0, 0, 0, 0)] = std::bind(&Renderer::SHIFT, this);
}

void Renderer::LMB_CLICK()
{
	if (IsClick()) {
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	//glClearDepth(-10.0f);


	if (is_outliner_selected) {
		if (pre_act_go_ID != 0 && obj_list.find(pre_act_go_ID) != obj_list.end())
			obj_list[pre_act_go_ID]->is_selected = false;

		if (obj_list.find(active_GO_ID) != obj_list.end()) {
			obj_list[active_GO_ID]->is_selected = true;
			active_shader = obj_list[active_GO_ID]->GetShaderStruct();
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
////////////////////////////////////// START RENDERING ///////////////////////////////////////////////
void Renderer::Render(bool rend, bool buff) {

	if (cam_list.find(0) == cam_list.end())_ASSERT("NONE ACTIVE CAMERA");
	if (envir_list.find(0) == envir_list.end())_ASSERT("NONE ACTIVE ENVIRONMENT");

	//GLDEBUG
	glEnable(GL_BLEND);

	if (buff)
		envir_list[0]->BindFrameBuffer();

	UpdateFrame();
	if (rend) {
		//glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		////////////    MESHES    ////////////
		cam_list[0]->ApplyTransform();
		cam_list[0]->GetInvTransform();
		cam_list[0]->GenFloatData();

		//DEBUG(viewport_offset)

			envir_list[0]->envir_hdr.Bind(HDR_TEXTURE);
			for (const auto& obj : mesh_list)
			{

				if (!obj.second->is_viewport)continue;

				obj.second->ApplyTransform();
				if (is_light_changed || obj.second->o_shader->is_shader_changed)
				{
					obj.second->RenderObj(cam_list[0], light_list);
				}
				else {
					obj.second->RenderObj(cam_list[0], emptyLight);
				}
				obj.second->is_Uniform_changed = false;
				obj.second->o_shader->is_shader_changed = false;
			}
			is_light_changed = false;
			envir_list[0]->envir_hdr.Unbind();

		//glDisable(GL_STENCIL_TEST);
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
	}

	if (buff) {
		envir_list[0]->UnBindFrameBuffer();
		//glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		//glStencilMask(0x00);
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		framebuffer->BindFrameBuffer();
		envir_list[0]->RenderEnvironment(cam_list[0], active_GO_ID * (int)(!is_spirit_selected));
		framebuffer->UnbindFrameBuffer();
	}
	//DEBUG(is_spirit_selected)
	Reset();
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

void Renderer::Reset()
{
	////////////  RESET  ///////////

	if (multi_select) {
		multi_select = false;
		if (selec_list.size())
			selec_list.clear();
	}
	cam_list[0]->is_invUniform_changed = false;
	cam_list[0]->is_frustum_changed = false;
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
		spirit_list[light->light_spirit.GetObjectID()] = &light->light_spirit;
		outline_list.push_back(OutlineElement(light->o_type, light->light_spirit.GetObjectID(), light->o_name, 0));
		parent_index_list.push_back(-1);
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
		name_buff[envir->GetObjectID()] = envir->o_name;
		outline_list.push_back(OutlineElement(envir->o_type, envir->GetObjectID(), envir->o_name, 0));
		parent_index_list.push_back(-1);
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
