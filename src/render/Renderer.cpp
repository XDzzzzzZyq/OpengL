#include "Renderer.h"
#include "SDFField.h"
#include "SceneManager.h"
#include "Input.h"

#include "xdz_math.h"

#include "events/EditorEvents.h"

GLint Renderer::max_resolution_w = 0;
GLint Renderer::max_resolution_h = 0;

Renderer::Renderer()
	:r_frame_width(SCREEN_W), r_frame_height(SCREEN_H)
{}

void Renderer::Init(EventPool& evt)
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

	ComputeShader::InitComputeLib(GetConfig());
	Light::EnableShadowMap();

	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &max_resolution_w);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &max_resolution_h);

	evt.subscribe<RenderSurfaceResizedEvent>([this](const RenderSurfaceResizedEvent& e) {
		this->FrameResize(e.width, e.height );
		});

}

Renderer::~Renderer()
{}

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

void RenderShadowMap(Light* light, SceneResource::ResPool<Mesh> mesh_list, const RenderConfigs& config)
{
	//TODO: not necessary for every frame update
	const GLuint map_w = light->light_shadow_map.GetW();
	const GLuint map_h = light->light_shadow_map.GetH();

	glViewport(0, 0, map_w, map_h);

	light->BindShadowMapBuffer();
	light->BindShadowMapShader();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (const auto& [id, mesh] : mesh_list)
	{
		if (!mesh->using_shadow) continue;
		if (!mesh->is_viewport) continue;

		light->BindTargetTrans(mesh->o_Transform);
		mesh->RenderObjProxy();
	}

	FrameBuffer::UnbindFrameBuffer();

	if (config.RequiresMomentShadow()) {
		light->ConstructSAT(&config);
	}
}

void Renderer::Render(const SceneContext& ctx, bool rend, bool buff) {


	/* Check at least one camera and environment */
	SceneResource* scene = dynamic_cast<SceneResource*>(ctx.c_active_scene);
	if (scene->cam_list.find(0) == scene->cam_list.end()) assert(false && "NONE ACTIVE CAMERA");
	if (scene->envir_list.find(0) == scene->envir_list.end()) assert(false && "NONE ACTIVE ENVIRONMENT");

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	////////////    CAM CACHED    /////////////

	const Camera* cam = scene->GetActiveCamera();
	glm::mat4 proj_trans_b = cam->cam_frustum * cam->o_InvTransform;


	////////////  TRANSFORM UDPATE  /////////////
	
	scene->UpdateObjTransforms();
	if (scene->CheckStatus(SceneResource::SceneChanged) && r_config.r_sampling_average == RenderConfigs::SamplingType::Average)
		Input::ResetFrameCount(1);
	//scene->_debugStatus();
	

	////////////     OPTICAL FLOW     ////////////
	
	if (r_config.RequiresFwdOF())
	{
		ComputeShader& of = ComputeShader::ImportShader("Optical_Flow");
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(POS_B_FB, 0);
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(OPT_FLW_FB, 1);
		of.UseShader();
		of.SetValue("proj_trans", cam->cam_frustum * cam->o_InvTransform);
		of.RunComputeShaderSCR(r_render_result->GetSize(), 16);
	}

	
	/////////// Signed Distance Field ///////////

	const bool requires_sdf = r_config.RequiresSDF();
	const bool realtime_sdf = (!scene->CheckStatus(SceneResource::ObjectTransChanged)) || r_config.r_sampling_average == RenderConfigs::SamplingType::Average;
	if (requires_sdf && scene->CheckStatus(SceneResource::SDFChanged) && realtime_sdf)
		ConstructSDF(ctx);

	
	///////////  Lights Data PreCalc  ///////////

	for (auto& [id, light] : scene->light_list) {
		if (!light->is_viewport) return;

		if (light->is_light_changed || light->is_Uniform_changed) {
			r_light_data.UpdateLight(light.get());
		}

		if (light->is_light_changed || scene->CheckStatus(SceneResource::ObjectTransChanged)) {
			RenderShadowMap(light.get(), scene->mesh_list, r_config);
		}

		/* Depth Test for Shadow Map */
		if (light->is_Uniform_changed)
			light->UpdateProjMatrix();
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
		Environment* env = scene->GetActiveEnvironment();
		env->RenderEnvironment(ctx);
		glEnable(GL_DEPTH_TEST);

		
		////////////    MESHES    ////////////

		env->BindEnvironTexture();
		scene->sdf_field->Bind();
		for (const auto& [id, mesh] : scene->mesh_list)
		{
			if (!mesh->is_viewport)continue;
			mesh->RenderMesh(ctx);
		}
		
		/////////  POLYGONAL LIGHTS POLYGON    /////////

		for (const auto& [id, polyLight] : scene->poly_light_list)
		{
			if (!polyLight->is_viewport)continue;
			polyLight->RenderPolygon(ctx);
			if (polyLight->is_Uniform_changed)
				r_light_data.ParsePolygonLightData(scene->poly_light_list);
		}
		
		/////////    DEBUG MESHES    /////////

		for (const auto& [id, dLine] : scene->dLine_list)
		{
			if (!dLine->is_viewport)continue;
			dLine->RenderDdbugLine(ctx);
		}
		
		for (const auto& [id, dPoints] : scene->dPoints_list)
		{
			if (!dPoints->is_viewport)continue;
			dPoints->RenderDebugPoint(ctx);
		}
		

		////////////    ICONS    ////////////

		if (r_render_icons) {
			glEnable(GL_BLEND);
			for (const auto& [id, light] : scene->light_list)
			{
				if (!light->light_sprite.is_viewport)continue;
				light->RenderLightSpr(ctx);
			}
			for (const auto& [id, envir] : scene->envir_list) {
				if (!envir->envir_sprite.is_viewport)continue;
				envir->RenderEnvirSpr(ctx);
			}
			for (const auto& pps : scene->pps_list) {
				if (!pps->pps_sprite.is_viewport)continue;
				pps->RenderPPSSpr(ctx);
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
			ComputeShader& outline = ComputeShader::ImportShader("selection_outline");
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 0);
			if (ctx.c_selections.GetSelectedObjects() != nullptr) 
				outline.RunComputeShaderSCR(r_buffer_list[_RASTER].GetSize(), 16);
		}
		

		//////// BACKWARD OPTICAL FLOW ////////

		if (r_config.RequiresBwdOF())
		{
			ComputeShader& of_b = ComputeShader::ImportShader("Optical_Flow_Back");
			r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 0);
			r_buffer_list[_AO_ELS].BindFrameBufferTexR(OPT_FLW_FB, 1);
			of_b.UseShader();
			of_b.SetValue("proj_trans_b", proj_trans_b);
			of_b.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}
		

		////////////  SSAO + DEPTH  ////////////

		ComputeShader& ssao = ComputeShader::ImportShader(ComputeShader::GetAOShaderName(GetConfig()));
		float ao_update_rate = r_config.r_sampling_average == RenderConfigs::SamplingType::IncrementAverage ? 0.05f : 1.0f / Input::GetFrameCount();
		r_buffer_list[_AO_ELS].BindFrameBufferTex(OPT_FLW_FB, 1);
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(POS_B_FB, 2);
		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 3);
		r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 4);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
		r_buffer_list[_AO_ELS].BindFrameBufferTexR(LIGHT_AO_FB, 6);
		TextureLib::Noise_2D_16x16xN()->BindC(7);
		ssao.UseShader();
		if (cam->is_Uniform_changed) {
			ssao.SetValue("Cam_pos", cam->o_position);
			ssao.SetValue("Proj_Trans", cam->cam_frustum * cam->o_InvTransform);
		}
		ssao.SetValue("update_rate", ao_update_rate);
		ssao.SetValue("noise_level", Input::GetFrameCount() % 6);
		ssao.RunComputeShaderSCR(r_render_result->GetSize(), 16);

		
		//////////// LIGHTING CACHE ////////////

		const float shadow_update_rate = r_config.r_sampling_average == RenderConfigs::SamplingType::IncrementAverage ? 0 : 1.0f / Input::GetFrameCount();
		r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 2);
		r_buffer_list[_RASTER].BindFrameBufferTexR(POS_FB, 3);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 5);
		r_buffer_list[_AO_ELS].BindFrameBufferTex(OPT_FLW_FB, 6);
		if (r_config.RequiresSDF()) scene->sdf_field->Bind();
		r_light_data.UpdateLightingCache(Input::GetFrameCount(), GetConfig());
		

		////////////  PBR COMPOSE  ////////////

		//r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("point_far", Light::point_shaodow_far);
		scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("U_Shadow", r_light_data.GetTotalCount(), LightArrayBuffer::shadow_slot, VEC1_ARRAY);
		r_buffer_list[_RASTER].BindFrameBufferTex(AVAIL_PASSES);
		TextureLib::LTC1()->Bind(13);
		TextureLib::LTC2()->Bind(14);
		r_light_data.Bind();
		r_buffer_list[_RASTER].BindFrameBufferTex(POS_FB,		BUFFER_TEXTURE + POS_FB);
		r_buffer_list[_RASTER].BindFrameBufferTex(NORMAL_FB,	BUFFER_TEXTURE + NORMAL_FB);
		r_buffer_list[_RASTER].BindFrameBufferTex(ALBEDO_FB,	BUFFER_TEXTURE + ALBEDO_FB);
		r_buffer_list[_RASTER].BindFrameBufferTex(MRSE_FB,		BUFFER_TEXTURE + MRSE_FB);
		r_buffer_list[_RASTER].BindFrameBufferTex(MASK_FB,		BUFFER_TEXTURE + MASK_FB);
		r_buffer_list[_RASTER].BindFrameBufferTex(EMIS_COL_FB,	BUFFER_TEXTURE + EMIS_COL_FB);

		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_render_result->BindFrameBufferTexR(DIR_DIFF_FB, 1);
		r_render_result->BindFrameBufferTexR(DIR_SPEC_FB, 2);
		r_render_result->BindFrameBufferTexR(IND_DIFF_FB, 3);
		r_render_result->BindFrameBufferTexR(IND_SPEC_FB, 4);
		r_render_result->BindFrameBufferTexR(DIR_EMIS_FB, 5);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 7);
		scene->pps_list[_PBR_COMP_PPS]->SetShaderValue("Cam_pos", cam->o_position);
		r_render_result->BindFrameBuffer();
		scene->pps_list[_PBR_COMP_PPS]->RenderPPS(r_render_result->GetSize(), 16);
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
			scene->sdf_field->Bind();
			ssr.UseShader();
			ssr.SetValue("use_incr_aver", (bool)r_config.r_sampling_average);
			ssr.SetValue("std_ud_rate", 1.0f / Input::GetFrameCount());
			ssr.SetValue("cam_pos", cam->o_position);
			ssr.SetValue("cam_trans", cam->cam_frustum * cam->o_InvTransform);
			ssr.SetValue("noise", Input::input_state.random.random_float1);
			ssr.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}

		
		////////////     FXAA     ////////////

		if (r_config.RequiresFXAA()) {
			ComputeShader& fxaa = ComputeShader::ImportShader(ComputeShader::GetAAShaderName(GetConfig()));
			r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
			r_buffer_list[_RASTER].BindFrameBufferTexR(RAND_FB, 1);
			r_buffer_list[_RASTER].BindFrameBufferTexR(NORMAL_FB, 2);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 3);
			fxaa.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}

		
		//////////  COLOR ADJUSTMENT  /////////

		ComputeShader& tone = ComputeShader::ImportShader("pps/Compose", Uni("U_debugt", 3));
		r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
		r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
		//r_buffer_list[_RASTER].BindFrameBufferTexR(RAND_FB, 3);
		//r_render_result->BindFrameBufferTexR(DIR_DIFF_FB, 2);
		//r_buffer_list[_RASTER].BindFrameBufferTex(MASK_FB, 1);
		tone.UseShader();
		tone.SetValue("gamma", r_config.r_gamma);
		tone.RunComputeShaderSCR(r_render_result->GetSize(), 8);
		

		//////////   EDITING ELEM   //////////

		if (r_is_preview)
		{
			ComputeShader& editing = ComputeShader::ImportShader("pps/Editing");
			r_render_result->BindFrameBufferTexR(COMBINE_FB, 0);
			r_buffer_list[_RASTER].BindFrameBufferTexR(MASK_FB, 1);
			editing.RunComputeShaderSCR(r_render_result->GetSize(), 16);
		}
	}
}

void Renderer::ConstructSDF(const SceneContext& ctx)
{
	SceneResource* scene = dynamic_cast<SceneResource*>(ctx.c_active_scene);
	scene->sdf_field->Bind();
	scene->sdf_field->ResetDistance();

	scene->sdf_field->BindShader();

	for (const auto& [id, mesh] : scene->mesh_list)
	{
		if (!mesh->using_sdf) continue;
		if (!mesh->is_viewport) continue;

		scene->sdf_field->BindTargetTrans(mesh->o_Transform, mesh->is_closure);
		mesh->RenderObjProxy(false);
	}

	scene->SetSceneStatus(SceneResource::SDFChanged, false);

	scene->sdf_field->Unbind();
	scene->sdf_field->UnbindShader();
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
}

void Renderer::FrameResize(GLuint _w, GLuint _h)
{
	r_frame_width = _w;
	r_frame_height = _h;

	FrameBufferResize({ (float)_w, (float)_h });

	r_light_data.Resize(_w, _h);
}

void Renderer::ScreenShot()
{
	std::string name = "result""-" + std::to_string(Input::input_state.random.random_float1);
	DEBUG("saving to: " + name);
	r_render_result->GetFBTexturePtr(COMBINE_FB)->SaveTexture(name, true);
}
