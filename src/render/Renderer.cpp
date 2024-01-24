#include "Renderer.h"
#include "xdz_math.h"

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
	r_sdf_field = SDFField(64, 64, 64);
	r_sdf_field.SetScale({ 1.1,1.1,1.1 });
	r_sdf_field.SetPos({ 0,0,1 });
	r_sdf_field.ResetBuffer();

	EventInit();
	ComputeShader::InitComputeLib(GetConfig());
	Light::EnableShadowMap();

	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &max_resolution_w);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &max_resolution_h);
}

Renderer::~Renderer()
{
	for (auto& buffer : r_buffer_list)
		buffer.Del();
}

std::string Renderer::GetObjectName(int ID)
{
	if (r_scene->obj_list.find(ID) == r_scene->obj_list.end())
		return "None";

	return r_scene->obj_list[ID]->o_name;
}

int Renderer::GetSelectID(GLuint x, GLuint y)
{
	if (viewport_offset - glm::vec2(5, 5) < glm::vec2(x, y) && glm::vec2(x, y) < viewport_offset + r_buffer_list[_RASTER].GetFrameBufferSize() * glm::vec2(1, 2))
		//return GetActiveEnvironment()->envir_frameBuffer->ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
		return r_buffer_list[_RASTER].ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
	else
		return EventListener::active_GO_ID;
}

void Renderer::InitFrameBuffer()
{
	r_render_result = std::make_shared<FrameBuffer>(std::vector<FBType>RESULT_PASSES);
	r_buffer_list.emplace_back(std::vector<FBType>AVAIL_PASSES);
	r_buffer_list.emplace_back(std::vector<FBType>{ LIGHT_AO_FB, POS_B_FB, OPT_FLW_FB });
}

void Renderer::BindFrameBuffer(int slot)
{
	r_buffer_list[slot].BindFrameBuffer();
}

void Renderer::EndFrameBuffer(int slot)
{
	r_buffer_list[slot].UnbindFrameBuffer();
}

void Renderer::FrameBufferResize(const glm::vec2& size)
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

	EventListener::GetActiveObject = [&](int id) { return r_scene->obj_list[id].get(); };
	EventListener::GetActiveCamera = [&]()		 { return dynamic_cast<GameObject*>(GetActiveCamera().get()); };
}

void Renderer::LMB_CLICK()
{
	if (!EventListener::IsMouseClick()) return;
	if (!EventListener::is_in_viewport) return;
	if (EventListener::viewport_status != EventListener::ViewPortStatus::None) return;

	int id = GetSelectID(mouse_x, mouse_y);
	if (id == EventListener::active_GO_ID) return;

	if (r_scene->obj_list.find(EventListener::active_GO_ID) != r_scene->obj_list.end())
		r_scene->obj_list[EventListener::active_GO_ID]->is_selected = false;

	EventListener::active_GO_ID = id;

	if (r_scene->obj_list.find(EventListener::active_GO_ID) != r_scene->obj_list.end()) {
		r_scene->obj_list[EventListener::active_GO_ID]->is_selected = true;
		EventListener::active_object = EventListener::GetActiveObject(EventListener::active_GO_ID);
	}
	else {
		EventListener::active_GO_ID = 0;
		EventListener::active_object = nullptr;
	}


	EventListener::is_selected_changed = true;

	for (auto [id, _] : r_scene->sprite_list)
		is_sprite_selected |= id == active_GO_ID;
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

	if (r_scene->cam_list.find(0) == r_scene->cam_list.end()) assert(false && "NONE ACTIVE CAMERA");
	if (r_scene->envir_list.find(0) == r_scene->envir_list.end()) assert(false && "NONE ACTIVE ENVIRONMENT");

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	////////////    CAM CACHED    /////////////

	glm::mat4 proj_trans_b = GetActiveCamera()->cam_frustum * GetActiveCamera()->o_InvTransform;


	////////////  TRANSFORM UDPATE  /////////////

	r_scene->UpdateObjTransforms();
	if (r_scene->CheckStatus(SceneResource::SceneChanged) && r_config.r_sampling_average == RenderConfigs::SamplingType::Average)
		EventListener::frame_count = 1;
	//r_scene->_debugStatus();


	////////////     OPTICAL FLOW     ////////////

	if (r_config.RequiresFwdOF())
	{
		static ComputeShader& of = ComputeShader::ImportShader("Optical_Flow");
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(POS_B_FB, 0);
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(OPT_FLW_FB, 1);
		of.UseShader();
		of.SetValue("proj_trans", GetActiveCamera()->cam_frustum * GetActiveCamera()->o_InvTransform);
		of.RunComputeShaderSCR(r_render_result->GetSize(), 16);
	}


	/////////// Signed Distance Field ///////////

	const bool requires_sdf = r_config.RequiresSDF();
	const bool realtime_sdf = (!r_scene->CheckStatus(SceneResource::ObjectTransChanged)) || r_config.r_sampling_average == RenderConfigs::SamplingType::Average;
	if(requires_sdf && r_scene->CheckStatus(SceneResource::SDFChanged) && realtime_sdf)
		ConstructSDF();


	///////////  Lights Data PreCalc  ///////////

	for (auto& [id, light] : r_scene->light_list) {
		if (!light->is_viewport) return;

		if (light->is_light_changed || light->is_Uniform_changed)
			r_light_data.UpdateLight(light.get());

		/* Depth Test for Shadow Map */

		if (light->is_Uniform_changed)
			light->UpdateProjMatrix();

		RenderShadowMap(light.get());
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

		glDisable(GL_DEPTH_TEST);
		GetActiveEnvironment()->RenderEnvironment(GetActiveCamera().get());
		glEnable(GL_DEPTH_TEST);


		////////////    MESHES    ////////////

		GetActiveEnvironment()->BindEnvironTexture();
		r_sdf_field.Bind();
		for (const auto& [id, mesh] : r_scene->mesh_list)
		{
			if (!mesh->is_viewport)continue;
			mesh->RenderMesh(GetActiveCamera().get());
		}

		/////////  POLYGONAL LIGHTS POLYGON    /////////

		for (const auto& [id, polyLight] : r_scene->poly_light_list)
		{
			if (!polyLight->is_viewport)continue;
			polyLight->RenderPolygon(GetActiveCamera().get());
			if (polyLight->is_Uniform_changed)
				r_light_data.ParsePolygonLightData(r_scene->poly_light_list);
		}

		/////////    DEBUG MESHES    /////////

		for (const auto& [id, dLine] : r_scene->dLine_list)
		{
			if (!dLine->is_viewport)continue;
			dLine->RenderDdbugLine(GetActiveCamera().get());
		}

		for (const auto& [id, dPoints] : r_scene->dPoints_list)
		{
			if (!dPoints->is_viewport)continue;
			dPoints->RenderDebugPoint(GetActiveCamera().get());;
		}


		////////////    ICONS    ////////////

		if (r_render_icons) {
			glEnable(GL_BLEND);
			for (const auto& [id, light] : r_scene->light_list)
			{
				if (!light->light_sprite.is_viewport)continue;
				light->RenderLightSpr(GetActiveCamera().get());
			}
			for (const auto& [id, envir] : r_scene->envir_list) {
				if (!envir->envir_sprite.is_viewport)continue;
				envir->RenderEnvirSpr(GetActiveCamera().get());
			}
			for (const auto& pps : r_scene->pps_list) {
				if (!pps->pps_sprite.is_viewport)continue;
				pps->RenderPPSSpr(GetActiveCamera().get());
			}
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

		if (r_is_preview) {
			static ComputeShader& outline = ComputeShader::ImportShader("selection_outline");
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 0);
			if (active_GO_ID != 0) outline.RunComputeShaderSCR(r_buffer_list[_RASTER].GetSize(), 16);
		}


		//////// BACKWARD OPTICAL FLOW ////////

		if (r_config.RequiresBwdOF())
		{
			static ComputeShader& of_b = ComputeShader::ImportShader("Optical_Flow_Back");
			r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 0);
			r_buffer_list[_AO_ELS].BindFrameBufferTexR(OPT_FLW_FB, 1);
			of_b.UseShader();
			of_b.SetValue("proj_trans_b", proj_trans_b);
			of_b.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}


		////////////  SSAO + DEPTH  ////////////

		static ComputeShader& ssao = ComputeShader::ImportShader(ComputeShader::GetAOShaderName(GetConfig()));
		float ao_update_rate = r_config.r_sampling_average == RenderConfigs::SamplingType::IncrementAverage ? 0.05f : 1.0 / EventListener::frame_count;
		r_buffer_list[_AO_ELS].BindFrameBufferTex(OPT_FLW_FB, 1);
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(POS_B_FB, 2);
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
		ssao.SetValue("update_rate", ao_update_rate);
		ssao.SetValue("noise_level", EventListener::frame_count % 6);
		ssao.RunComputeShaderSCR(r_render_result->GetSize(), 16);


		//////////// LIGHTING CACHE ////////////

		if (r_config.RequiresShadow()) {
			const float shadow_update_rate = r_config.r_sampling_average == RenderConfigs::SamplingType::IncrementAverage ? 0 : 1.0 / EventListener::frame_count;
			r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 3);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
			r_buffer_list[_AO_ELS].BindFrameBufferTex(OPT_FLW_FB, 6);
			r_light_data.UpdateLightingCache(EventListener::frame_count, GetConfig());
		}


		////////////  PBR COMPOSE  ////////////

		//r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		r_scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("point_far", Light::point_shaodow_far);
		r_scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("U_Shadow", r_light_data.GetTotalCount(), LightArrayBuffer::shadow_slot, VEC1_ARRAY);
		r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		TextureLib::LTC1()->Bind(13);
		TextureLib::LTC2()->Bind(14);
		r_light_data.Bind();
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_render_result->BindFrameBufferTexR(DIR_DIFF_FB, 1);
		r_render_result->BindFrameBufferTexR(DIR_SPEC_FB, 2);
		r_render_result->BindFrameBufferTexR(IND_DIFF_FB, 3);
		r_render_result->BindFrameBufferTexR(IND_SPEC_FB, 4);
		r_render_result->BindFrameBufferTexR(DIR_EMIS_FB, 5);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 7);
		r_scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("Cam_pos", GetActiveCamera()->o_position);
		r_render_result->BindFrameBuffer();
		r_scene->pps_list[_PBR_COMP_PPS]->RenderPPS(r_render_result->GetSize(), 16);		
		r_render_result->UnbindFrameBuffer();


		////////////      SSR     ////////////

		if (r_config.RequiresSSR()) {
			static std::vector<glm::vec3> noise = xdzm::rand3nv(32);
			ComputeShader& ssr = ComputeShader::ImportShader(ComputeShader::GetSSRShaderName(GetConfig()));
			r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
			r_buffer_list[_RASTER].BindFrameBufferTex(POS_FB, 1);
			r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 2);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MRSE_FB, 3);
			r_buffer_list[_RASTER].BindFrameBufferTexR(ALBEDO_FB, 4);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
			r_buffer_list[_AO_ELS].BindFrameBufferTex(LIGHT_AO_FB, 6);
			r_render_result->BindFrameBufferTex(DIR_DIFF_FB, 7);
			r_render_result->BindFrameBufferTex(DIR_SPEC_FB, 8);
			r_render_result->BindFrameBufferTex(IND_DIFF_FB, 9);
			r_render_result->BindFrameBufferTex(IND_SPEC_FB, 10);
			r_render_result->BindFrameBufferTex(DIR_EMIS_FB, 11);
			r_sdf_field.Bind();
			ssr.UseShader();
			ssr.SetValue("use_incr_aver", (bool)r_config.r_sampling_average);
			ssr.SetValue("std_ud_rate", 1.0f / EventListener::frame_count);
			ssr.SetValue("cam_pos", GetActiveCamera()->o_position);
			ssr.SetValue("cam_trans", GetActiveCamera()->cam_frustum * GetActiveCamera()->o_InvTransform);
			ssr.SetValue("noise", EventListener::random_float1);
			ssr.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}


		////////////     FXAA     ////////////

		if (r_config.RequiresFXAA()) {
			static ComputeShader& fxaa = ComputeShader::ImportShader(ComputeShader::GetAAShaderName(GetConfig()));
			r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
			r_buffer_list[_RASTER].BindFrameBufferTexR(RAND_FB, 1);
			r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 2);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 3);
			fxaa.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}


		//////////  COLOR ADJUSTMENT  /////////

		static ComputeShader& tone = ComputeShader::ImportShader("Compose", Uni("U_debugt", 3));
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
		//r_render_result->BindFrameBufferTexR(DIR_DIFF_FB, 2);
		//r_buffer_list[_RASTER].BindFrameBufferTex(MASK_FB, 1);
		tone.UseShader();
		tone.SetValue("gamma", r_config.r_gamma);
		tone.RunComputeShaderSCR(r_render_result->GetSize(), 8);


		//////////   EDITING ELEM   //////////

		if (r_is_preview)
		{
			static ComputeShader& editing = ComputeShader::ImportShader("Editing");
			r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
			editing.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}

	}
}

void Renderer::RenderShadowMap(Light* light)
{
	const GLuint map_w = light->light_shadow_map.GetW();
	const GLuint map_h = light->light_shadow_map.GetH();

	glViewport(0, 0, map_w, map_h);

	light->BindShadowMapBuffer();
	light->BindShadowMapShader();

	glClear(GL_DEPTH_BUFFER_BIT);

	for (const auto& [id, mesh] : r_scene->mesh_list)
	{
		if (!mesh->using_shadow) continue;
		if (!mesh->is_viewport) continue;

		light->BindTargetTrans(mesh->o_Transform);
		mesh->RenderObjProxy();
	}

	FrameBuffer::UnbindFrameBuffer();
}

void Renderer::ConstructSDF()
{
	r_sdf_field.Bind();
	r_sdf_field.ResetDistance();

	r_sdf_field.BindShader();

	for (const auto& [id, mesh] : r_scene->mesh_list)
	{
		if (!mesh->using_sdf) continue;
		if (!mesh->is_viewport) continue;

		r_sdf_field.BindTargetTrans(mesh->o_Transform, mesh->is_closure);
		mesh->RenderObjProxy(false);
	}

	r_scene->SetSceneStatus(SceneResource::SDFChanged, false);

	r_sdf_field.Unbind();
	r_sdf_field.UnbindShader();
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

	r_scene->ResetStatus();
}

void Renderer::FrameResize(GLuint _w, GLuint _h)
{
	r_frame_width = _w;
	r_frame_height = _h;

	FrameBufferResize({ (float)_w, (float)_h });
	r_light_data.Resize(_w, _h);
}




void Renderer::UseScene(std::shared_ptr<SceneResource> _scene)
{
	r_scene = _scene;

	r_light_data.ParseLightData(_scene->light_list);
	r_light_data.ParsePolygonLightData(_scene->poly_light_list);
}

void Renderer::ActivateCamera(int cam_id)
{
	if (r_scene->cam_list.find(cam_id) == r_scene->cam_list.end())
		return;

	is_GOlist_changed = true;
	r_scene->cam_list[0] = r_scene->cam_list[cam_id];
}

std::shared_ptr<Camera> Renderer::GetActiveCamera()
{
	return r_scene->GetActiveCamera();
}

void Renderer::ActivateEnvironment(int envir_id)
{
	if (r_scene->envir_list.find(envir_id) == r_scene->envir_list.end())
		return;

	is_GOlist_changed = true;
	r_scene->envir_list[0] = r_scene->envir_list[envir_id];
}

std::shared_ptr<Environment> Renderer::GetActiveEnvironment()
{
	return r_scene->GetActiveEnvironment();
}

std::shared_ptr<PostProcessing> Renderer::GetPPS(int _tar)
{
	return r_scene->GetPPS(_tar);
}

void Renderer::ScreenShot()
{
	r_render_result->GetFBTexturePtr(COMBINE_FB)->SaveTexture("result""-" + std::to_string(EventListener::random_float1));
}
