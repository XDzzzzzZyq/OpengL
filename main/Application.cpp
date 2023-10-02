#include "Application.h"
#include "xdz_math.h"

Application::Application() {}


Application* Application::m_app;

Application* Application::Get()
{
	if (!m_app) {
		m_app = new Application();
	}
	return m_app;
}

Application::~Application()
{
	if (m_app) delete m_app;
}


int Application::Init()
{
	std::iostream::sync_with_stdio(false);
	std::cout << std::boolalpha;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_SAMPLES, 4);
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
	renderer.r_ao_radius = 0.8;
	renderer.r_render_icons = false;

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
		UI.GetParaValue("test layer", "Roughness")->para_data.fdata = xdzm::rand11();
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

	UI.FindImguiMenuItem("Render", "Optical Flow")->BindSwitch(&renderer.r_using_of);
	UI.FindImguiMenuItem("Render", "Forward Optical Flow")->BindSwitch(&renderer.r_forward_of);
	UI.FindImguiMenuItem("Render", "Deferred Rendering")->BindSwitch(&renderer.r_deferred_rendering);
	UI.FindImguiMenuItem("Render", "FXAA")->BindSwitch(&renderer.r_using_fxaa);
	UI.FindImguiMenuItem("Render", "Screen Space Reflection")->BindSwitch(&renderer.r_using_ssr);
	UI.FindImguiMenuItem("Render", "Shadow Map")->BindSwitch(&renderer.r_using_shadow_map);
	UI.FindImguiMenuItem("Render", "SDF Field")->BindSwitch(&renderer.r_using_SDF_field);
	UI.FindImguiMenuItem("Render", "Icons")->BindSwitch(&renderer.r_render_icons);

	UI.ParaUpdate = [&] {
		UI.FindImguiItem("__Parameters__", "MOUSE_POS : [%.1f : %.1f]")->SetArgsList(2, Event.mouse_x, Event.mouse_y);
		UI.FindImguiItem("__Parameters__", "Frame Rate %.3f ms/frame (%.1f FPS)")->SetArgsList(2, 1000.0f / AvTime.result, AvTime.result);

		scale = UI.GetParaValue("__Parameters__", "SCALE")->para_data.fdata;
		power = UI.GetParaValue("__Parameters__", "POWER")->para_data.fdata;
		rotateX = UI.GetParaValue("__Parameters__", "X")->para_data.fdata;
		rotateY = UI.GetParaValue("__Parameters__", "Y")->para_data.fdata;
		rotateZ = UI.GetParaValue("__Parameters__", "Z")->para_data.fdata;
		Radius_W = UI.GetParaValue("__Parameters__", "W")->para_data.fdata;
		LightColor = UI.GetParaValue("__Parameters__", "Light Color")->para_data.v3data;
		LightPos = UI.GetParaValue("__Parameters__", "Light Position")->para_data.v3data;
		LightRot = UI.GetParaValue("__Parameters__", "Light Rotation")->para_data.v3data;
		testf = UI.GetParaValue("test layer", "testf")->para_data.fdata;
		Metalness = UI.GetParaValue("test layer", "Metalness")->para_data.fdata;
		Roughness = UI.GetParaValue("test layer", "Roughness")->para_data.fdata;
		Specular = UI.GetParaValue("test layer", "Specular")->para_data.fdata;
		//renderer.GetActiveEnvironment()->envir_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->para_data.fdata;
		renderer.r_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->para_data.fdata;
	};
	UI.GetCurrentWindow();

	EventListener::ShowEvents();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Update here */
		renderer.r_frame_num++;
		UI.NewFrame();

		Event.UpdateEvent(window);
		UI.RenderUI();

		AvTime.Update(UI.GetIO()->Framerate);

		renderer.EventActivate();
		renderer.GetActiveCamera()->EventActivate();

		/* Render here */

#if 0
		renderer.GetActiveCamera()->ChangeCamPersp(70 + rotateX * 3);
#endif

		renderer.Render();
		Event.Reset();

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
