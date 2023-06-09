#include "Application.h"

Application::Application()
{

}

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
	/* Make the window's context current */
	return 0;
}

int Application::Run()
{	
	DEBUG("\n---------------CAMERA----------------")
		std::shared_ptr<Camera> camera = std::make_shared<Camera>(10.0f, 10.0f, 70, 0.1f, 300.0f);
	camera->SetPos({ 0.0f, 20.0f, 0.0f });
	camera->SetRot({ 90, 0, 180 });
	camera->ApplyTransform();
	camera->GetInvTransform();
	renderer.UseCamera(camera);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go1 = std::make_shared<Mesh>("res/obj/monkey2.obj");
	go1->SetObjShader("testS", "Rasterization");
	go1->SetTex("avatar2.png", RGBA_TEXTURE);
	go1->SetCenter();
	go1->ApplyTransform();
	renderer.UseMesh(go1);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go2 = std::make_shared<Mesh>("res/obj/torus.obj");
	go2->SetObjShader("testS", "Rasterization");
	go2->SetTex("avatar1.png", RGBA_TEXTURE);
	go2->SetCenter();
	go2->SetPos({ 8, 0, 0 });
	go2->SetScale(glm::vec3(1.5f));
	go2->ApplyTransform();
	go1->SetParent(go2->GetTransformPtr());
	renderer.UseMesh(go2);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go3 = std::make_shared<Mesh>("res/obj/UVsphere.obj");
	go3->SetObjShader("testS", "Rasterization");
	go3->SetPos({ -8,0,0 });
	go3->SetScale({ 3,3,3 });
	renderer.UseMesh(go3);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> pointLight1 = std::make_shared<Light>(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight1->SetPos({ 2.0f, 2.0f, 2.0f });
	pointLight1->ApplyTransform();
	renderer.UseLight(pointLight1);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> pointLight2 = std::make_shared<Light>(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight2->SetRadius(2);
	renderer.UseLight(pointLight2);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> sunLight1 = std::make_shared<Light>(SUNLIGHT, 1.0f, glm::vec3(1.0f));
	sunLight1->SetRot(glm::vec3(45));
	sunLight1->SetPos(glm::vec3(2));
	sunLight1->SetPower(30);
	renderer.UseLight(sunLight1);

	DEBUG("\n---------------LINE----------------")
		std::shared_ptr<DebugLine> line = std::make_shared<DebugLine>();
	line->PushDebugLine(5, 5, 5);
	renderer.UseDebugLine(line);

	DEBUG("\n---------------ENVIR----------------")
		std::shared_ptr<Environment> environment = std::make_shared<Environment>("hdr/room.hdr");
	environment->SetPos(glm::vec3(0.0f, 7.0f, 7.0f));
	renderer.UseEnvironment(environment);

	DEBUG("\n---------------POINT----------------")
		std::shared_ptr<DebugPoints> points = std::make_shared<DebugPoints>();
	points->PushDebugPoint(5, 5, 5);
	renderer.UseDebugPoints(points);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps1 = std::make_shared<PostProcessing>("PBR");
	pps1->AddBinding("U_color",				BUFFER_TEXTURE + COMBINE_FB);
	pps1->AddBinding("U_pos",				BUFFER_TEXTURE + POS_FB);
	pps1->AddBinding("U_normal",			BUFFER_TEXTURE + NORMAL_FB);
	pps1->AddBinding("U_albedo",			BUFFER_TEXTURE + ALBEDO_FB);
	pps1->AddBinding("U_mrse",				BUFFER_TEXTURE + MRSE_FB);
	pps1->AddBinding("U_emission",			BUFFER_TEXTURE + EMIS_COL_FB);
	pps1->AddBinding("U_alpha",				BUFFER_TEXTURE + MASK_FB);
	pps1->AddBinding("Envir_Texture_diff",	IBL_TEXTURE);
	pps1->AddBinding("Envir_Texture_spec",	IBL_TEXTURE + 1);
	pps1->AddBinding("LUT",					PNG_TEXTURE);
	renderer.UsePostProcessing(pps1);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps2 = std::make_shared<PostProcessing>("Post_Visual");
	pps2->AddBinding("U_combine",			BUFFER_TEXTURE + COMBINE_FB);
	pps2->AddBinding("U_select",			BUFFER_TEXTURE + MASK_FB);
	renderer.UsePostProcessing(pps2);

	DEBUG("-------------------------------")
		/////////////////////////////////

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

	static float scale = 0.3f;
	static float blend = 0.5f;
	static float rotateX = 0.0f;
	static float rotateY = 0.0f;
	static float rotateZ = 0.0f;
	static float Radius;
	double mouse_x = 0.0f, mouse_y = 0.0f;
	ImVec4 LightColor = ImVec4(1.0f, 0.5f, 0.5f, 1.00f);
	ImVec4 LightPos = ImVec4(0.7f, 0.7f, 1.0f, 1.00f);
	ImVec4 LightRot = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	AverageTime<10> AvTime;
	int tex_type = 0;
	static float Metalness = 0.0f;
	static float Roughness = 0.0f;
	static float Specular = 1.0f;


	UI.SetButtonFunc("__Parameters__", "Debug", [&] {
		tex_type++;
		if (tex_type >= MAX_FB)tex_type = 0;
		renderer.GetActiveEnvironment()->SwapFrameBuffer((FBType)(tex_type));
		renderer.GetPPS(0)->SetShaderValue("U_color", BUFFER_TEXTURE + tex_type);
		renderer.r_using_fxaa = !renderer.r_using_fxaa;
		if(EventListener::active_shader) DEBUG(EventListener::active_shader->shader_struct_list[1].Main)
		//DEBUG(renderer.r_using_fxaa)
		});
	UI.SetButtonFunc("test layer", "testB", [&] {
		glm::vec3 newpoint2 = xdzm::rand3n(8.65f);
		points->PushDebugPoint(newpoint2);
		line->PushDebugLine(newpoint2);
		UI.GetParaValue("test layer", "Roughness")->para_data.fdata = xdzm::rand11();
		//go1.o_shader->ShaderLibDebug();
		//environment->envir_shader->ShaderLibDebug();
		//environment->envir_IBL_diff.GenIrradiaceConvFrom(environment->envir_IBL_spec);
		});
	UI.FindImguiLayer("Viewport")->resize_event = [&] {
		ImVec2 view_size = UI.FindImguiLayer("Viewport")->uly_size + ImVec2(10, 10);
		if ((int)view_size.x % 2)
			view_size.x++;
		glViewport(0, 0, view_size.x, view_size.y);
		camera->ChangeCamRatio(view_size);
		renderer.FrameBufferResize(0, view_size);
		UI.FindImguiItem("Viewport", "Viewport")->ResetSize(view_size);
		UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetFrameBufferTexture(0));
		//UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetActiveEnvironment()->envir_frameBuffer->GetFBTextureID(ID_FB));
	};
	UI.FindImguiLayer("CompShader")->resize_event = [&] {
		UI.FindImguiItem("CompShader", "Viewport")->ResetSize(UI.FindImguiLayer("CompShader")->uly_size + ImVec2(10, 10));
		UI.FindImguiItem("CompShader", "Viewport")->ResetBufferID(renderer.GetActiveEnvironment()->envir_IBL_spec.GetTexID());
	};
	UI.ParaUpdate = [&] {
		UI.FindImguiItem("__Parameters__", "MOUSE_POS : [%.1f : %.1f]")->SetArgsList(2, Event.mouse_x, Event.mouse_y);
		UI.FindImguiItem("__Parameters__", "Frame Rate %.3f ms/frame (%.1f FPS)")->SetArgsList(2, 1000.0f / AvTime.result, AvTime.result);

		scale = UI.GetParaValue("__Parameters__", "SCALE")->para_data.fdata;
		blend = UI.GetParaValue("__Parameters__", "POWER")->para_data.fdata;
		rotateX = UI.GetParaValue("__Parameters__", "X")->para_data.fdata;
		rotateY = UI.GetParaValue("__Parameters__", "Y")->para_data.fdata;
		rotateZ = UI.GetParaValue("__Parameters__", "Z")->para_data.fdata;
		Radius = UI.GetParaValue("__Parameters__", "W")->para_data.fdata;
		LightColor = UI.GetParaValue("__Parameters__", "Light Color")->para_data.v3data;
		LightPos = UI.GetParaValue("__Parameters__", "Light Position")->para_data.v3data;
		LightRot = UI.GetParaValue("__Parameters__", "Light Rotation")->para_data.v3data;
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
		AvTime.Update(UI.GetIO()->Framerate);
		renderer.EventActivate();

		/* Render here */

		go1->SetScale(0.7f * glm::vec3(scale));
		go1->SetRot(glm::vec3(0.0f, 0.0f, renderer.r_frame_num / 25.0f));
		//go1.SetPos(ImVec4_vec3(LightPos, 10.0f));

		//go2->SetPos(ImVec4_vec3_Uni(LightColor, 10.0f) + glm::vec3(8, 0, 0));
		go2->SetScale(glm::vec3(blend * 3));
		go2->SetRot(ImVec4_vec3_Uni(LightRot, 90.0f));

		go3->SetShaderValue("blen", Roughness);
		go3->SetShaderValue("metalness", Metalness);
		go3->SetShaderValue("specular", Specular);

		renderer.GetActiveCamera()->EventActivate();
		renderer.GetActiveCamera()->ChangeCamPersp(70 + rotateX * 3);

		pointLight1->SetColor(LightColor);
		pointLight1->SetPos(ImVec4_vec3_Uni(LightPos, 10.0f));
		pointLight1->SetPower(rotateY + 10);
		pointLight1->SetRadius(Radius);

		pointLight2->SetPos(ImVec4_vec3_Uni(LightPos, -10.0f));
		pointLight2->SetPower(rotateZ + 10);

		sunLight1->SetRot1D<2>(Radius*360);

		line->SetPos(glm::vec3(rotateX, 0, 0));
		line->dLine_color = glm::vec3(1, (90 - rotateY) / 90, (90 - rotateZ) / 90);

		renderer.Render();
		UI.RenderUI();
		Event.Reset();
		//GLDEBUG
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	DEBUG(std::to_string(1000 / AvTime.result) + "ms")
	std::cout << std::endl << "[ Finished ]" << std::endl;
	std::cout << GameObject::count << " object(s)" << std::endl;

	return 0;
}

int Application::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
