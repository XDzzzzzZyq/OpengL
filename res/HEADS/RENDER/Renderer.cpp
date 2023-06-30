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
	if(r_scene->obj_list.find(ID) == r_scene->obj_list.end())
		return "None";

	return r_scene->obj_list[ID]->o_name;
}

int Renderer::GetSelectID(GLuint x, GLuint y)
{
	if (viewport_offset - ImVec2(5, 5) < ImVec2(x, y) && ImVec2(x, y) < viewport_offset + r_buffer_list[_RASTER].GetFrameBufferSize() * ImVec2(1, 2))
		//return GetActiveEnvironment()->envir_frameBuffer->ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
		return r_buffer_list[_RASTER].ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
	else
		return EventListener::active_GO_ID;
}

void Renderer::InitFrameBuffer()
{
	r_render_result = std::make_shared<FrameBuffer>(std::vector<FBType>RESULT_PASSES);
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

	EventListener::GetActiveObject = [&](int id) { return r_scene->obj_list[id].get(); };
}

void Renderer::LMB_CLICK()
{
	if (!EventListener::IsMouseClick()) return;
	if (!EventListener::is_in_viewport) return;

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

	///////////  Lights Data PreCalc  ///////////

	for (auto& [id, light] : r_scene->light_list) {

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
		for (const auto& [id, mesh] : r_scene->mesh_list)
		{
			if (!mesh->is_viewport)continue;

			mesh->ApplyAllTransform();
			mesh->RenderObj(GetActiveCamera().get());
			mesh->is_Uniform_changed = false;
			mesh->o_shader->is_shader_changed = false;
			mesh->o_material->is_mat_struct_changed = false;
		}
		is_light_changed = false;

		/////////  POLYGONAL LIGHTS POLYGON    /////////

		for (const auto& [id, polyLight] : r_scene->poly_light_list)
		{
			polyLight->ApplyAllTransform();
			polyLight->RenderPolygon(GetActiveCamera().get());
			if (polyLight->is_Uniform_changed)
				r_light_data.ParsePolygonLightData(r_scene->poly_light_list);
			polyLight->is_Uniform_changed = false;
			polyLight->o_shader->is_shader_changed = false;
		}

		/////////    DEBUG MESHES    /////////

		for (const auto& [id, dLine] : r_scene->dLine_list)
		{
			if (!dLine->is_viewport)continue;
			dLine->ApplyAllTransform();
			dLine->RenderDdbugLine(GetActiveCamera().get());
			dLine->is_Uniform_changed = false;
		}

		for (const auto& [id, dPoints] : r_scene->dPoints_list)
		{
			if (!dPoints->is_viewport)continue;
			dPoints->ApplyAllTransform();
			dPoints->RenderDebugPoint(GetActiveCamera().get());
			dPoints->is_Uniform_changed = false;
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

		ComputeShader& outline = ComputeShader::ImportShader("selection_outline");
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 0);
		if (active_GO_ID != 0) outline.RunComputeShaderSCR(r_buffer_list[_RASTER].GetSize(), 16);


		////////////  SSAO + DEPTH  ////////////

		static std::vector<glm::vec3> kernel = xdzm::rand3hKernel(r_ao_ksize);
		ComputeShader& ssao = ComputeShader::ImportShader("SSAO", Uni("incre_average", true), Uni("kernel_length", (GLuint)r_ao_ksize), Uni("kernel", (GLuint)r_ao_ksize, (float*)kernel.data(), VEC3_ARRAY), Uni("noise_size", 16), Uni("update_rate", 0.05f), Uni("radius", r_ao_radius));
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
		ssao.RunComputeShaderSCR(r_render_result->GetSize(), 16);


		//////////// LIGHTING CACHE ////////////

		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 3);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
		r_light_data.UpdateLightingCache(r_frame_num);


		////////////  PBR COMPOSE  ////////////

		r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		r_scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("point_far", Light::point_shaodow_far);
		r_scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("U_Shadow", r_light_data.GetTotalCount(), LightArrayBuffer::shadow_slot, VEC1_ARRAY);
		TextureLib::IBL_LUT()->Bind(PNG_TEXTURE);
		TextureLib::LTC1()->Bind(13);
		TextureLib::LTC2()->Bind(14);
		r_light_data.Bind();
		if (GetActiveCamera()->is_Uniform_changed)
			r_scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("Cam_pos", GetActiveCamera()->o_position);
		r_render_result->BindFrameBuffer();
		r_scene->pps_list[_PBR_COMP_PPS]->RenderPPS();

		// store render result
		r_render_result->UnbindFrameBuffer();


		////////////      SSR     ////////////

		static std::vector<glm::vec3> noise = xdzm::rand3nv(32);
		ComputeShader& ssr = ComputeShader::ImportShader("SSR", Uni("U_pos", 1), Uni("U_dir_diff", 7), Uni("U_dir_spec", 8), Uni("U_ind_diff", 9), Uni("U_ind_spec", 10), Uni("U_emission", 11));
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
		ssr.UseShader();
		if (GetActiveCamera()->is_Uniform_changed) {
			ssr.SetValue("cam_pos", GetActiveCamera()->o_position);
			ssr.SetValue("cam_trans", GetActiveCamera()->cam_frustum * GetActiveCamera()->o_InvTransform);
		}
		ssr.SetValue("gamma", r_gamma);
		ssr.SetValue("noise", EventListener::random_float1);
		if (r_using_ssr) ssr.RunComputeShaderSCR(r_render_result->GetSize(), 16);


		////////////     FXAA     ////////////

		ComputeShader& fxaa = ComputeShader::ImportShader("FXAA");
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(RAND_FB, 1);
		r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 2);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 3);
		if (r_using_fxaa) fxaa.RunComputeShaderSCR(r_render_result->GetSize(), 16);


		//////////   EDITING ELEM   //////////

		ComputeShader& editing = ComputeShader::ImportShader("Editing");
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
		editing.RunComputeShaderSCR(r_render_result->GetSize(), 16);
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

	for (const auto& [id, mesh] : r_scene->mesh_list) {
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
	return r_scene->cam_list[0];
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
	return r_scene->envir_list[0];
}

std::shared_ptr<PostProcessing> Renderer::GetPPS(int _tar)
{
	if (_tar >= r_scene->pps_list.size())
		return nullptr;

	return r_scene->pps_list[_tar];
}

void Renderer::ScreenShot()
{
	r_render_result->GetFBTexturePtr(COMBINE_FB)->SaveTexture("result""-" + std::to_string(EventListener::random_float1));
}
