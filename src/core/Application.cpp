#include "Application.h"
#include "xdz_math.h"

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
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		return -1;
	}
	glfwMakeContextCurrent(window);
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	renderer.Init();
	UI.Init();
	Event.SetWindow(window);

	MeshLib::MeshLibInit();

	UI.SetConfigFlag(ImGuiConfigFlags_DockingEnable);
	UI.SetConfigFlag(ImGuiConfigFlags_ViewportsEnable);
	UI.SetBackendFlag(ImGuiBackendFlags_PlatformHasViewports);
	UI.SetBackendFlag(ImGuiBackendFlags_PlatformHasViewports);

	// 	if (UI.GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	// 	{
	// 		UI.GetStyle().WindowRounding = 0.0f;
	// 		UI.GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
	// 	}

	UI.ManagerInit(window);


#if 0
	renderer.UseScene(SceneManager::SceneConfig3());
	renderer.r_using_shadow_map = false;
	renderer.r_using_ssr = false;
#else
	renderer.UseScene(SceneManager::SceneConfig2());
	renderer.GetConfig()->r_ao_radius = 0.8;
	//renderer.r_render_icons = false;

	Light::area_blur_range = 0.03;
#endif

	/* Make the window's context current */
	return 0;
}

int Application::Run()
{
	DEBUG("-------------------------------")
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
		tex_type++;
		if (tex_type >= MAX_FB)tex_type = 0;
		renderer.GetActiveEnvironment()->SwapFrameBuffer((FBType)(tex_type));
		renderer.GetPPS(0)->SetShaderValue("U_color", BUFFER_TEXTURE + tex_type);
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
	UI.FindImguiLayer("Viewport")->resize_event = [&] {
		ImVec2 view_size = UI.FindImguiLayer("Viewport")->uly_size + ImVec2(10, 10);
		renderer.GetActiveCamera()->ChangeCamRatio(view_size.x, view_size.y);
		renderer.FrameResize(view_size.x, view_size.y);
		UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetFrameBufferTexture(0));
		//UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetActiveEnvironment()->envir_frameBuffer->GetFBTextureID(ID_FB));
	};
	Texture temp{};
	UI.FindImguiLayer("CompShader")->resize_event = [&] {
		UI.FindImguiItem("CompShader", "Viewport")->ResetBufferID(temp.GetTexID());
	};

	dynamic_cast<Viewport*>(UI.FindImguiLayer("Viewport"))->display_grid = false;
	UI.FindImguiMenuItem("Render", "Rendering PipeLine")->BindOption(&renderer.GetConfig()->r_pipeline);
	UI.FindImguiMenuItem("Render", "Optical Flow")->BindOption(&renderer.GetConfig()->r_of_algorithm);
	UI.FindImguiMenuItem("Render", "Anti Aliasing")->BindOption(&renderer.GetConfig()->r_anti_alias);
	UI.FindImguiMenuItem("Render", "Screen Space Reflection")->BindOption(&renderer.GetConfig()->r_ssr_algorithm);
	UI.FindImguiMenuItem("Render", "Shadow")->BindOption(&renderer.GetConfig()->r_shadow_algorithm);
	UI.FindImguiMenuItem("Render", "Ambient Occlusion")->BindOption(&renderer.GetConfig()->r_ao_algorithm);
	UI.FindImguiMenuItem("Render", "Sampling")->BindOption(&renderer.GetConfig()->r_sampling_average);
	UI.FindImguiMenuItem("View", "Icons")->BindSwitch(&renderer.r_render_icons);

	UI.ParaUpdate = [&] {
		UI.FindImguiItem("__Parameters__", "MOUSE_POS : [%.1f : %.1f]")->SetArgsList(2, Event.mouse_x, Event.mouse_y);
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
		//renderer.GetActiveEnvironment()->envir_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->Get<float>();
		renderer.GetConfig()->r_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->Get<float>();
	};
	UI.GetCurrentWindow();

	EventListener::ShowEvents();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Update here */
		UI.NewFrame();

		Event.UpdateEvent(window);
		AvTime.Update(UI.GetIO()->Framerate);

		UI.EventActivate();
		renderer.EventActivate();

		UI.RenderUI();

		renderer.GetActiveCamera()->EventActivate();
		/* Render here */

#if 0
		renderer.GetActiveCamera()->ChangeCamPersp(70 + rotateX * 3);
#endif
		
		renderer.Render();

		renderer.Reset();
		Event.Reset();

		//DEBUG(renderer.r_frame_count);
#if 0
		DEBUG(renderer.GetActiveCamera()->o_position);
		DEBUG(renderer.GetActiveCamera()->cam_pers);
#endif

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	DEBUG(std::to_string(1000 / AvTime.result) + "ms");
	std::cout << std::endl << "[ Finished ]" << std::endl;
	std::cout << GameObject::count << " object(s)" << std::endl;

	return 0;
}

int Application::Terminate()
{
	ComputeShader::ResetComputeLib();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
