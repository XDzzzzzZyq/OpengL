#include "Renderer.h"

GLint Renderer::max_resolution_w = 0;
GLint Renderer::max_resolution_h = 0;

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


	DEBUG("Renderer Open");
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

	InitFrameBuffer();
	r_light_data.Init();

	EventInit();

	Light::EnableShadowMap();

	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH , &max_resolution_w);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &max_resolution_h);
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

void Renderer::InitFrameBuffer()
{
	r_render_result = FrameBuffer(std::vector<FBType>RESULT_PASSES);
	r_buffer_list.emplace_back(std::vector<FBType>AVAIL_PASSES);
	r_buffer_list.emplace_back(std::vector<FBType>{ LIGHT_AO_FB });
}

void Renderer::BindFrameBuffer(int slot)
{
	r_buffer_list[slot].BindFrameBuffer();
}

void Renderer::EndFrameBuffer(int slot)
{
	r_buffer_list[slot].UnbindFrameBuffer();
}

void Renderer::FrameBufferResize(const ImVec2& size)
{
	for (auto& buffer : r_buffer_list)
		buffer.Resize(size);
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

	for (auto i : sprite_id_buff) {
		if (i == active_GO_ID) {
			is_sprite_selected = true;
			return;
		}
		else {
			is_sprite_selected = false;
		}

	}


}

void Renderer::SHIFT()
{
	multi_select = true;
}

//////////////////////////////////////////////

void Renderer::NewFrame()
{
	glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, r_frame_width, r_frame_height);

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


	/* Check at least one camera and environment */

	if (cam_list.find(0) == cam_list.end()) assert(false && "NONE ACTIVE CAMERA");
	if (envir_list.find(0) == envir_list.end()) assert(false && "NONE ACTIVE ENVIRONMENT");

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	///////////  Lights Data PreCalc  ///////////

	for (auto& [id, light] : light_list) {

		/*	    Capture Status		*/
		light->ApplyAllTransform();
		is_light_changed |= light->is_light_changed;
		is_light_changed |= light->is_Uniform_changed;

		if (light->is_light_changed || light->is_Uniform_changed)
			r_light_data.UpdateLight(light.get());

		/* Depth Test for Shadow Map */

		if (light->is_Uniform_changed)
			light->UpdateProjMatrix();

		RenderShadowMap(light.get());
		light->is_light_changed = false;
		light->is_Uniform_changed = false;
	}

	///////////   Begin buffering    ///////////

	if (buff) {
		//GetActiveEnvironment()->BindFrameBuffer();
		r_buffer_list[_RASTER].BindFrameBuffer();
	}

	NewFrame();

	if (rend) {
		//glEnable(GL_STENCIL_TEST);
		;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_BLEND);

		/////////    CAM TRANSFORM    /////////

		GetActiveCamera()->ApplyTransform();
		GetActiveCamera()->GetInvTransform();
		GetActiveCamera()->GenFloatData();

		glDisable(GL_DEPTH_TEST);
		GetActiveEnvironment()->RenderEnvironment(GetActiveCamera().get());
		glEnable(GL_DEPTH_TEST);


		////////////    MESHES    ////////////

		GetActiveEnvironment()->BindEnvironTexture();
		for (const auto& mesh : mesh_list)
		{
			if (!mesh.second->is_viewport)continue;

			mesh.second->ApplyAllTransform();
			mesh.second->RenderObj(GetActiveCamera().get());
			mesh.second->is_Uniform_changed = false;
			mesh.second->o_shader->is_shader_changed = false;
		}
		is_light_changed = false;

		/////////  POLYGONAL LIGHTS POLYGON    /////////

		for (const auto& polyLight : poly_light_list)
		{
			polyLight.second->ApplyAllTransform();
			polyLight.second->RenderPolygon(GetActiveCamera().get());
			if (polyLight.second->is_Uniform_changed)
				r_light_data.ParsePolygonLightData(poly_light_list);
			polyLight.second->is_Uniform_changed = false;
			polyLight.second->o_shader->is_shader_changed = false;
		}

		/////////    DEBUG MESHES    /////////

		for (const auto& dLine : dLine_list)
		{
			if (!dLine.second->is_viewport)continue;
			dLine.second->ApplyAllTransform();
			dLine.second->RenderDdbugLine(GetActiveCamera().get());
			dLine.second->is_Uniform_changed = false;
		}

		for (const auto& dPoints : dPoints_list)
		{
			if (!dPoints.second->is_viewport)continue;
			dPoints.second->ApplyAllTransform();
			dPoints.second->RenderDebugPoint(GetActiveCamera().get());
			dPoints.second->is_Uniform_changed = false;
		}


		////////////    ICONS    ////////////

		glEnable(GL_BLEND);
		for (const auto& light : light_list)
		{
			if (!light.second->light_sprite.is_viewport)continue;
			light.second->RenderLightSpr(GetActiveCamera().get());
		}
		for (const auto& envir : envir_list) {
			if (!envir.second->envir_sprite.is_viewport)continue;
			envir.second->RenderEnvirSpr(GetActiveCamera().get());
		}
		for (const auto& pps : pps_list) {
			if (!pps->pps_sprite.is_viewport)continue;
			pps->RenderPPSSpr(GetActiveCamera().get());
		}
	}
	if (buff) {
		//GetActiveEnvironment()->UnbindFrameBuffer();
		r_buffer_list[_RASTER].UnbindFrameBuffer();


		///////     BEGIN PROCESSING    ///////

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//GetActiveEnvironment()->envir_frameBuffer->BindFrameBufferTex(AVAIL_PASSES);


		////////////    OUTLINE    ////////////

		ComputeShader& outline = ComputeShader::ImportShader("selection_outline");
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 0);
		if (active_GO_ID != 0) outline.RunComputeShader(r_buffer_list[_RASTER].GetSize() / 8);


		////////////  SSAO + DEPTH  ////////////

		static std::vector<glm::vec3> kernel = xdzm::rand3hKernel(r_ao_ksize);
		ComputeShader& ssao = ComputeShader::ImportShader("SSAO", Uni("incre_average", true), Uni("kernel_length", (GLuint)r_ao_ksize), Uni("kernel", (GLuint)r_ao_ksize, (float*)kernel.data(), VEC3_ARRAY), Uni("noise_size", 16), Uni("update_rate", 0.05f));
		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 3);
		r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 4);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(LIGHT_AO_FB, 6);
		TextureLib::Noise_2D_16x16()->BindC(7, GL_READ_ONLY, 1);
		ssao.UseShader();
		if (GetActiveCamera()->is_Uniform_changed) {
			ssao.SetValue("Cam_pos", GetActiveCamera()->o_position);
			ssao.SetValue("Proj_Trans", GetActiveCamera()->cam_frustum * GetActiveCamera()->o_InvTransform);
		}
		ssao.SetValue("noise_level", r_frame_num % 6);
		ssao.RunComputeShader(r_render_result->GetSize() / 8);


		//////////// LIGHTING CACHE ////////////

		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 3);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
		r_light_data.UpdateLightingCache(r_frame_num);


		////////////  PBR COMPOSE  ////////////

		r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		pps_list[_PBR_COMP_PPS]->SetShaderValue("point_far", Light::point_shaodow_far);
		pps_list[_PBR_COMP_PPS]->SetShaderValue("U_Shadow", r_light_data.GetTotalCount(), LightArrayBuffer::shadow_slot, VEC1_ARRAY);
		TextureLib::IBL_LUT()->Bind(PNG_TEXTURE);
		TextureLib::LTC1()->Bind(13);
		TextureLib::LTC2()->Bind(14);
		r_light_data.Bind();
		if (GetActiveCamera()->is_Uniform_changed)
			pps_list[_PBR_COMP_PPS]->SetShaderValue("Cam_pos", GetActiveCamera()->o_position);
		r_render_result->BindFrameBuffer();
		pps_list[_PBR_COMP_PPS]->RenderPPS();

		// store render result
		r_render_result->UnbindFrameBuffer();


		////////////      SSR     ////////////

		ComputeShader& ssr = ComputeShader::ImportShader("SSR", Uni("U_dir_diff", 7), Uni("U_dir_spec", 8), Uni("U_ind_diff", 9), Uni("U_ind_spec", 10));
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 1);
		r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 2);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MRSE_FB, 3);
		r_buffer_list[_RASTER].BindFrameBufferTexR(ALBEDO_FB, 4);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
		r_buffer_list[_AO_ELS].BindFrameBufferTex(LIGHT_AO_FB, 6);
		r_render_result->BindFrameBufferTex(DIR_DIFF_FB, 7);
		r_render_result->BindFrameBufferTex(DIR_SPEC_FB, 8);
		r_render_result->BindFrameBufferTex(IND_DIFF_FB, 9);
		r_render_result->BindFrameBufferTex(IND_SPEC_FB, 10);
		ssr.UseShader();
		if (GetActiveCamera()->is_Uniform_changed) {
			ssr.SetValue("cam_pos", GetActiveCamera()->o_position);
			ssr.SetValue("cam_trans", GetActiveCamera()->cam_frustum * GetActiveCamera()->o_InvTransform);
		}
		ssr.SetValue("gamma", r_gamma);
		if (r_using_ssr) ssr.RunComputeShader(r_render_result->GetSize() / 8);


		////////////     FXAA     ////////////

		ComputeShader& fxaa = ComputeShader::ImportShader("FXAA");
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 1);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 2);
		if (r_using_fxaa) fxaa.RunComputeShader(r_render_result->GetSize() / 4);


		//////////   EDITING ELEM   //////////

		ComputeShader& editing = ComputeShader::ImportShader("Editing");
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
		editing.RunComputeShader(r_render_result->GetSize() / 4);
	}
	Reset();
}

void Renderer::RenderShadowMap(Light* light)
{
	const GLuint map_w = light->light_shadow_map.GetW();
	const GLuint map_h = light->light_shadow_map.GetH();

	glViewport(0, 0, map_w, map_h);

	light->BindShadowMapBuffer();
	light->BindShadowMapShader();

	glClear(GL_DEPTH_BUFFER_BIT);

	for (const auto& [id, mesh] : mesh_list) {
		if(!mesh->using_shadow) continue;

		light->BindTargetTrans(mesh->o_Transform);
		mesh->RenderObjProxy();
	}

	FrameBuffer::UnbindFrameBuffer();
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
	GetActiveCamera()->is_Uniform_changed = false;
	GetActiveCamera()->is_invUniform_changed = false;
	GetActiveCamera()->is_frustum_changed = false;
	is_light_changed = false;
}

void Renderer::FrameResize(GLuint _w, GLuint _h)
{
	r_frame_width = _w;
	r_frame_height = _h;

	FrameBufferResize({ (float)_w, (float)_h });
	r_light_data.Resize(_w, _h);
}




void Renderer::UseCamera(std::shared_ptr<Camera> camera)
{
	if (cam_list.find(camera->GetObjectID()) == cam_list.end())
	{
		cam_list[0] = camera;
		return;
	}

	is_GOlist_changed = true;
	cam_list[camera->GetObjectID()] = camera;
	obj_list[camera->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(camera);
	cam_list[0] = camera;
	outline_list.push_back(OutlineElement(camera->o_type, camera->GetObjectID(), camera->o_name, 0));
	parent_index_list.push_back(-1);
	name_buff[camera->GetObjectID()] = camera->o_name;
}

void Renderer::UseCamera(const int& cam_id)
{
	if (cam_list.find(cam_id) == cam_list.end())
		return;

	is_GOlist_changed = true;
	cam_list[0] = cam_list[cam_id];
}

std::shared_ptr<Camera> Renderer::GetActiveCamera()
{
	return cam_list[0];
}

//////////////////////////////////////////////
void Renderer::UseMesh(std::shared_ptr<Mesh> mesh)
{
	if (mesh_list.find(mesh->GetObjectID()) != mesh_list.end())
		return;

	is_GOlist_changed = true;
	mesh_list[mesh->GetObjectID()] = mesh;
	obj_list[mesh->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(mesh);
	outline_list.push_back(OutlineElement(mesh->o_type, mesh->GetObjectID(), mesh->o_name, 0));
	parent_index_list.push_back(-1);
	name_buff[mesh->GetObjectID()] = mesh->o_name;

}

//////////////////////////////////////////////
void Renderer::UseLight(std::shared_ptr<Light> light)
{
	if (light_list.find(light->GetObjectID()) != light_list.end())
		return;

	is_GOlist_changed = true;
	is_light_changed = true;
	light_list[light->GetObjectID()] = light;
	obj_list[light->light_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(light);

	outline_list.push_back(OutlineElement(light->o_type, light->light_sprite.GetObjectID(), light->o_name, 0));
	parent_index_list.push_back(-1);

	sprite_list[light->light_sprite.GetObjectID()] = std::shared_ptr<Sprite>(light, &light->light_sprite);
	name_buff[light->light_sprite.GetObjectID()] = light->o_name; //using sprite ID
	sprite_id_buff.push_back(light->light_sprite.GetObjectID());

	r_light_data.ParseLightData(light_list);
}

void Renderer::UsePolygonLight(std::shared_ptr<PolygonLight> polyLight)
{
	if (poly_light_list.find(polyLight->GetObjectID()) != poly_light_list.end())
		return;

	is_GOlist_changed = true;
	poly_light_list[polyLight->GetObjectID()] = polyLight;
	is_GOlist_changed = true;
	obj_list[polyLight->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(polyLight);
	outline_list.push_back(OutlineElement(polyLight->o_type, polyLight->GetObjectID(), polyLight->o_name, 0));
	parent_index_list.push_back(-1);
	name_buff[polyLight->GetObjectID()] = polyLight->o_name;

	r_light_data.ParsePolygonLightData(poly_light_list);
}

void Renderer::UseEnvironment(std::shared_ptr<Environment> envir)
{
	if (envir_list.find(envir->GetObjectID()) != envir_list.end())
	{
		envir_list[0] = envir;
		return;
	}

	is_GOlist_changed = true;
	envir_list[envir->GetObjectID()] = envir;
	envir_list[0] = envir;
	name_buff[envir->envir_sprite.GetObjectID()] = envir->o_name;
	obj_list[envir->envir_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(envir);

	outline_list.push_back(OutlineElement(envir->o_type, envir->envir_sprite.GetObjectID(), envir->o_name, 0));
	parent_index_list.push_back(-1);

	sprite_list[envir->envir_sprite.GetObjectID()] = std::shared_ptr<Sprite>(envir, &envir->envir_sprite);
	name_buff[envir->envir_sprite.GetObjectID()] = envir->o_name; //using sprite ID
	sprite_id_buff.push_back(envir->envir_sprite.GetObjectID());
}

void Renderer::UseEnvironment(const int& envir_id)
{
	if (envir_list.find(envir_id) == envir_list.end())
		return;

	is_GOlist_changed = true;
	envir_list[0] = envir_list[envir_id];

}

std::shared_ptr<Environment> Renderer::GetActiveEnvironment()
{
	return envir_list[0];
}

void Renderer::UseDebugLine(std::shared_ptr<DebugLine> dline)
{
	if (dLine_list.find(dline->GetObjectID()) != dLine_list.end())
		return;

	is_GOlist_changed = true;
	dLine_list[dline->GetObjectID()] = dline;
	obj_list[dline->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(dline);
	outline_list.push_back(OutlineElement(dline->o_type, dline->GetObjectID(), dline->o_name, 0));
	name_buff[dline->GetObjectID()] = dline->o_name;
	parent_index_list.push_back(-1);

}

void Renderer::UseDebugPoints(std::shared_ptr<DebugPoints> dpoints)
{
	if (dPoints_list.find(dpoints->GetObjectID()) != dPoints_list.end())
		return;

	is_GOlist_changed = true;
	dPoints_list[dpoints->GetObjectID()] = dpoints;
	obj_list[dpoints->GetObjectID()] = std::dynamic_pointer_cast<GameObject>(dpoints);
	outline_list.push_back(OutlineElement(dpoints->o_type, dpoints->GetObjectID(), dpoints->o_name, 0));
	name_buff[dpoints->GetObjectID()] = dpoints->o_name;
	parent_index_list.push_back(-1);
}

void Renderer::UsePostProcessing(std::shared_ptr<PostProcessing> pps)
{
	pps_list.emplace_back(pps);
	name_buff[pps->pps_sprite.GetObjectID()] = pps->o_name;
	obj_list[pps->pps_sprite.GetObjectID()] = std::dynamic_pointer_cast<GameObject>(pps);

	outline_list.push_back(OutlineElement(pps->o_type, pps->pps_sprite.GetObjectID(), pps->o_name, 0));
	parent_index_list.push_back(-1);

	sprite_list[pps->pps_sprite.GetObjectID()] = std::shared_ptr<Sprite>(pps, &pps->pps_sprite);
	name_buff[pps->pps_sprite.GetObjectID()] = pps->o_name; //using sprite ID
	sprite_id_buff.push_back(pps->pps_sprite.GetObjectID());
}

std::shared_ptr<PostProcessing> Renderer::GetPPS(int _tar)
{
	if (_tar >= pps_list.size())
		return nullptr;

	return pps_list[_tar];
}
