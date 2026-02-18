#include "Application.h"
#include "SceneManager.h"

#include "controllers/CameraController.h"
#include "controllers/ViewportController.h"
#include "controllers/ShaderController.h"

#include "events/EditorEvents.h"
#include "events/MaterialEvents.h"

#include "layer/Viewport.h"

#include "xdz_math.h"
#include "shaders/ComputeShader.h"

Application& Application::Get()
{
	static std::unique_ptr<Application> instance_ptr(new Application());
	return *instance_ptr.get();
}

int Application::Init()
{
	std::iostream::sync_with_stdio(false);
	std::cout << std::boolalpha;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_W + 100, SCREEN_H, "TEST_WINDOW", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	// Editor Layer
	renderer.Init(EventPool);
	MeshLib::MeshLibInit();

	// Controllers
	Controllers.RegisterController<CameraController>(EventPool);
	Controllers.RegisterController<ViewportController>(EventPool);
	Controllers.RegisterController<ShaderController>(EventPool);

	// Context
	Ctx.Init(EventPool);

	// UI Layer
	UI.Init(EventPool);

	UI.SetConfigFlag(ImGuiConfigFlags_DockingEnable);
	//UI.SetConfigFlag(ImGuiConfigFlags_ViewportsEnable);

	// 	if (UI.GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	// 	{
	// 		UI.GetStyle().WindowRounding = 0.0f;
	// 		UI.GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
	// 	}

#if 0
	renderer.UseScene(SceneManager::SceneConfig3());
	renderer.r_using_shadow_map = false;
	renderer.r_using_ssr = false;
#else
	Ctx.scene.UseScene(SceneManager::Shadow().get());

	renderer.GetConfig()->r_ao_radius = 0.8f;
	Light::area_blur_range = 0.03f;

	SceneResource* scene = dynamic_cast<SceneResource*>(Ctx.scene.active_scene);
	EventPool.emit<FrameBufferResetEvent>({ &renderer.r_buffer_list[0], renderer.GetFrameBufferPtr() });
	EventPool.emit<RenderConfigChangedEvent>({ renderer.GetConfig(), ModifyFlags::ShadowChanged});
	for (const auto& [id, mesh] : scene->mesh_list)
	{
		if(mesh->using_material)
			EventPool.emit<MaterialStructChangedEvent>({ dynamic_cast<ObjectID*>(mesh.get()), mesh->o_material.get() });
	}

	// TODO: event system
	renderer.r_light_data.ParseLightData(scene->light_list);
	renderer.r_light_data.ParsePolygonLightData(scene->poly_light_list);
	Ctx.render.config = renderer.GetConfig();
	//renderer.r_render_icons = false;


	
#endif

	/* Make the window's context current */
	return 0;
}

int Application::Run()
{
	DEBUG("-------------------------------");
		/////////////////////////////////

	static float scale = 0.3f;
	static float power = 0.5f;
	static float rotateX = 0.0f;
	static float rotateY = 0.0f;
	static float rotateZ = 0.0f;
	static float Radius_W;
	double mouse_x = 0.0f, mouse_y = 0.0f;
	glm::vec3 LightColor = glm::vec3(1.0f, 0.5f, 0.5f);
	glm::vec3 LightPos = glm::vec3(0.7f, 0.7f, 1.0f);
	glm::vec3 LightRot = glm::vec3(0.5f, 0.5f, 0.5f);
	AverageTime<10> AvTime;
	int tex_type = 0;
	static float testf;
	static float Metalness = 0.0f;
	static float Roughness = 0.0f;
	static float Specular = 1.0f;


	UI.SetButtonFunc("__Parameters__", "Debug", [&] {
		SceneResource* scene = dynamic_cast<SceneResource*>(Ctx.scene.active_scene);
		tex_type++;
		if (tex_type >= MAX_FB)tex_type = 0;
		scene->GetActiveEnvironment()->SwapFrameBuffer((FBType)(tex_type));
		scene->GetPPS(0)->SetShaderValue("U_color", BUFFER_TEXTURE + tex_type);
		//renderer.r_using_fxaa = !renderer.r_using_fxaa;
		renderer.ScreenShot();
		});
	UI.SetButtonFunc("test layer", "testB", [&] {
		glm::vec3 newpoint2 = xdzm::rand3n(8.65f);
		//points->PushDebugPoint(newpoint2);
		//line->PushDebugLine(newpoint2);
		UI.GetParaValue("test layer", "Roughness")->Get<float>() = xdzm::rand11();
		//go1.o_shader->ShaderLibDebug();
		//environment->envir_shader->ShaderLibDebug();
		//environment->envir_IBL_diff.GenIrradiaceConvFrom(environment->envir_IBL_spec);
		});

	UI.FindImguiLayerAs<Viewport>("Viewport")->display_grid = false;
	SceneResource* scene = dynamic_cast<SceneResource*>(Ctx.scene.active_scene);
	UI.FindImguiMenuItem("Render", "Rendering PipeLine")->BindOption(&renderer.GetConfig()->r_pipeline);
	UI.FindImguiMenuItem("Render", "Optical Flow")->BindOption(&renderer.GetConfig()->r_of_algorithm);
	UI.FindImguiMenuItem("Render", "Anti Aliasing")->BindOption(&renderer.GetConfig()->r_anti_alias);
	UI.FindImguiMenuItem("Render", "Screen Space Reflection")->BindOption(&renderer.GetConfig()->r_ssr_algorithm);
	UI.FindImguiMenuItem("Render", "Shadow")->BindOption(&renderer.GetConfig()->r_shadow_algorithm, [&](bool) -> bool { 
		EventPool.emit<RenderConfigChangedEvent>({ renderer.GetConfig(), ModifyFlags::ShadowChanged});
		return true; 
		});
	UI.FindImguiMenuItem("Render", "Ambient Occlusion")->BindOption(&renderer.GetConfig()->r_ao_algorithm);
	UI.FindImguiMenuItem("Render", "Sampling")->BindOption(&renderer.GetConfig()->r_sampling_average);
	UI.FindImguiMenuItem("View",   "Icons")->BindSwitch(&renderer.r_render_icons);

	UI.ParaUpdate = [&] {
		UI.FindImguiItem("__Parameters__", "MOUSE_POS : [%.1f : %.1f]")->SetArgsList(2, Input::GetMousePosX(), Input::GetMousePosY());
		UI.FindImguiItem("__Parameters__", "Frame Rate %.3f ms/frame (%.1f FPS)")->SetArgsList(2, 1000.0f / AvTime.result, AvTime.result);

		scale = UI.GetParaValue("__Parameters__", "SCALE")->Get<float>();
		power = UI.GetParaValue("__Parameters__", "POWER")->Get<float>();
		rotateX = UI.GetParaValue("__Parameters__", "X")->Get<float>();
		rotateY = UI.GetParaValue("__Parameters__", "Y")->Get<float>();
		rotateZ = UI.GetParaValue("__Parameters__", "Z")->Get<float>();
		Radius_W = UI.GetParaValue("__Parameters__", "W")->Get<float>();
		LightColor = UI.GetParaValue("__Parameters__", "Light Color")->Get<glm::vec3>();
		LightPos = UI.GetParaValue("__Parameters__", "Light Position")->Get<glm::vec3>();
		LightRot = UI.GetParaValue("__Parameters__", "Light Rotation")->Get<glm::vec3>();
		testf = UI.GetParaValue("test layer", "testf")->Get<float>();
		Metalness = UI.GetParaValue("test layer", "Metalness")->Get<float>();
		Roughness = UI.GetParaValue("test layer", "Roughness")->Get<float>();
		Specular = UI.GetParaValue("test layer", "Specular")->Get<float>();
		//renderer.GetActiveEnvironment()->envir_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->GOet<float>();GLDEBUG
		renderer.GetConfig()->r_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->Get<float>();

		//temp.GenERectMapFrom(renderer.r_scene->light_list.begin()->second->light_shadow_map);
		//DEBUG(renderer.r_scene->light_list.begin()->second->light_shadow_map.GetTexID())
		//UI.FindImguiItem("CompShader", "Viewport")->ResetBufferID(temp.GetTexID());
		//UI._debug();
	};

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Update here */
		UI.NewFrame();

		AvTime.Update(UI.GetIO()->Framerate);
		InputManager.UpdateState(window);
		EventPool.EmitGlobalEvent();

		UI.RenderUI(Ctx, EventPool);

		SceneResource* scene = dynamic_cast<SceneResource*>(Ctx.scene.active_scene);
		/* Render here */		
		renderer.Render(Ctx);

		renderer.Reset();
		scene->ResetStatus();

		//DEBUG(renderer.r_frame_count);
#if 0
		DEBUG(renderer.GetActiveCamera()->o_position);
		DEBUG(renderer.GetActiveCamera()->cam_pers);
#endif

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	}
	DEBUG(std::to_string(1000 / AvTime.result) + "ms");
	std::cout << std::endl << "[ Finished ]" << std::endl;
	std::cout << UID::GetTotalAllocated() << " object(s)" << std::endl;

	return 0;
}

int Application::Terminate()
{
	TextureLib::ResetTexLib();
	ComputeShader::ResetComputeLib();
	UI.Terminate();
	glfwTerminate();

	return 0;
}
