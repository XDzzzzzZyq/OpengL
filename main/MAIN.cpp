#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <fstream>
#include <string>
#include <numeric>

#include "support.h"
#include "Renderer.h"
#include "Camera.h"

#include "EventListener.h"
#include "DebugLine.h"
#include "DebugPoints.h"
#include "Environment.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImguiManager.h"

using std::cout;
using std::endl;
using std::string;



void render(GLFWwindow* window) {
	glfwMakeContextCurrent(window);
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	GLDEBUG
	Renderer renderer;
	ImguiManager UI(window);
	EventListener Event;
	
	DEBUG("\n---------------CAMERA----------------")
	Camera camera(10.0f, 10.0f, 70, 0.1f, 300.0f);
	camera.SetPos({ 0.0f, 0.0f, 20.0f });
	camera.ApplyTransform();
	camera.GetInvTransform();
	renderer.UseCamera(&camera);

	DEBUG("\n---------------MESH----------------")
		Mesh go1("res/obj/monkey2.obj");
	go1.SetObjShader("testS");
	go1.SetTex("res/tex/avatar2.png", RGBA_TEXTURE);
	go1.SetCenter();
	go1.ApplyTransform();
	renderer.UseMesh(&go1);

	DEBUG("\n---------------MESH----------------")
		Mesh go2("res/obj/torus.obj");
	go2.SetObjShader("testS");
	go2.SetTex("res/tex/avatar1.png", RGBA_TEXTURE);
	go2.SetCenter();
	go2.SetPos({ 8, 0, 0 });
	go2.SetScale(glm::vec3(1.5f));
	go2.ApplyTransform();
	go1.SetParent(go2.GetTransformPtr());
	renderer.UseMesh(&go2);

	DEBUG("\n---------------MESH----------------")
		Mesh go3("res/obj/UVsphere.obj");
	go3.SetObjShader("testS");
	go3.SetPos({ -8,0,0 });
	go3.SetScale({ 3,3,3 });
	renderer.UseMesh(&go3);

	DEBUG("\n---------------LIGHT----------------")
		Light pointLight1(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight1.SetPos({ 2.0f, 2.0f, 2.0f });
	pointLight1.ApplyTransform();
	pointLight1.GenFloatData();

	DEBUG("\n---------------LIGHT----------------")
		Light pointLight2(POINTLIGHT, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	pointLight2.GenFloatData();
	renderer.UseLight(&pointLight1);
	renderer.UseLight(&pointLight2);

	DEBUG("\n---------------LINE----------------")
		DebugLine line;
	line.PushDebugLine(5, 5, 5);
	renderer.UseDebugLine(&line);

	DEBUG("\n---------------ENVIR----------------")
		Environment environment("res/tex/hdr/room.hdr");
	environment.SetPos(glm::vec3(0.0f, 7.0f, 7.0f));
	renderer.UseEnvironment(&environment);

	DEBUG("\n---------------POINT----------------")
		DebugPoints points;
	points.PushDebugPoint(5, 5, 5);
	renderer.UseDebugPoints(&points);
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
	double mouse_x = 0.0f, mouse_y = 0.0f;
	ImVec4 LightColor = ImVec4(1.0f, 0.5f, 0.5f, 1.00f);
	ImVec4 LightPos = ImVec4(0.7f, 0.7f, 1.0f, 1.00f);
	ImVec4 LightRot = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	AverageTime<500> AvTime;
	float FrameCount = 0;
	int tex_type = 0;
	float testfloat[3] = { 0.0f,0.5f,1.0f };

	UI.SetButtonFunc("__Parameters__", "Debug", [&] {
		glm::vec3 newpoint1 = 8.65f * glm::normalize(glm::vec3(rand11(), rand11(), rand11()));
		points.PushDebugPoint(newpoint1);
		line.PushDebugLine(newpoint1);
		tex_type++;
		if (tex_type > 1)tex_type = 0;
		renderer.GetActiveEnvironment()->SwapFrameBuffer((FBType)(tex_type*2));		DEBUG(go1.o_position)
		});
	UI.SetButtonFunc("test layer", "testB", [&] {
		glm::vec3 newpoint2 = 8.65f * glm::normalize(glm::vec3(rand11(), rand11(), rand11()));
		points.PushDebugPoint(newpoint2);
		line.PushDebugLine(newpoint2);
		UI.GetParaValue("test layer", "test")->para_data.fdata = rand11();
		//go1.o_shader->ShaderLibDebug();
		environment.envir_shader->ShaderLibDebug();
		//ShaderStruct::Debug();
		});
	UI.FindImguiLayer("Viewport")->resize_event = [&] {
		ImVec2 view_size = UI.FindImguiLayer("Viewport")->uly_size + ImVec2(10, 10);
		if ((int)view_size.x % 2)
			view_size.x++;
		glViewport(0, 0, view_size.x, view_size.y);
		camera.ChangeCamRatio(view_size);
		renderer.FrameBufferResize(0, view_size);
		UI.FindImguiItem("Viewport", "Viewport")->ResetSize(view_size);
		UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetFrameBufferTexture(0));
		//UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetActiveEnvironment()->envir_frameBuffer->GetFBTextureID(ID_FB));
	};
	UI.FindImguiLayer("CompShader")->resize_event = [&] {
		UI.FindImguiItem("CompShader", "Viewport")->ResetSize(UI.FindImguiLayer("CompShader")->uly_size + ImVec2(10, 10));
		UI.FindImguiItem("CompShader", "Viewport")->ResetBufferID(renderer.GetActiveEnvironment()->envir_IBL_diff.GetTexID());
	};
	UI.ParaUpdate = [&] {
		FrameCount++;
		UI.FindImguiItem("__Parameters__", "MOUSE_POS : [%.1f : %.1f]")->SetArgsList(2, Event.mouse_x, Event.mouse_y);
		UI.FindImguiItem("__Parameters__", "Frame Rate %.3f ms/frame (%.1f FPS)")->SetArgsList(2, 1000.0f / AvTime.result, AvTime.result);

		scale = UI.GetParaValue("__Parameters__", "SCALE")->para_data.fdata;
		blend = UI.GetParaValue("__Parameters__", "POWER")->para_data.fdata;
		rotateX = UI.GetParaValue("__Parameters__", "X")->para_data.fdata;
		rotateY = UI.GetParaValue("__Parameters__", "Y")->para_data.fdata;
		rotateZ = UI.GetParaValue("__Parameters__", "Z")->para_data.fdata;
		LightColor = UI.GetParaValue("__Parameters__", "Light Color")->para_data.v3data;
		LightPos = UI.GetParaValue("__Parameters__", "Light Position")->para_data.v3data;
		LightRot = UI.GetParaValue("__Parameters__", "Light Rotation")->para_data.v3data;
		renderer.GetActiveEnvironment()->envir_gamma = UI.GetParaValue("__Parameters__", "GAMMA")->para_data.fdata;
	};
	UI.GetCurrentWindow();

	EventListener::ShowEvents();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Update here */
		FrameCount++;
		UI.NewFrame();
		Event.UpdateEvent(window);
		AvTime.Add(UI.GetIO().Framerate);
		renderer.EventActivate();

		/* Render here */

		go1.SetScale(0.7f*glm::vec3(scale));
		go1.SetRot(glm::vec3(0.0f, FrameCount / 25, 0.0f));
		//go1.SetPos(ImVec4_vec3(LightPos, 10.0f));

		go2.SetPos(ImVec4_vec3_Uni(LightColor, 10.0f) + glm::vec3(8, 0, 0));
		go2.SetScale(glm::vec3(blend * 3));
		go2.SetRot(ImVec4_vec3_Uni(LightRot, 90.0f));

		renderer.GetActiveCamera()->EventActivate();
		renderer.GetActiveCamera()->ChangeCamPersp(70 + rotateX * 3);

		pointLight1.SetColor(LightColor);
		pointLight1.SetPos(ImVec4_vec3_Uni(LightPos, 10.0f));
		pointLight1.light_power = blend * 50;
		pointLight1.GenFloatData();

		pointLight2.SetPos(ImVec4_vec3_Uni(LightPos, -10.0f));
		pointLight2.light_power = blend * 20;
		pointLight2.GenFloatData();

		renderer.is_light_changed = true;

		line.SetPos(glm::vec3(rotateX, 0, 0));
		line.dLine_color = glm::vec3(1, (90 - rotateY) / 90, (90 - rotateZ) / 90);

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
	cout << endl << "[ Finished ]" << endl;
	cout << GameObject::count << " object(s)" << endl;

}

int main()
{
	std::iostream::sync_with_stdio(false);
	std::cout << std::boolalpha;
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(SCREEN_W+100, SCREEN_H, "TEST_WINDOW", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		return -1;
	}

	/* Create a windowed mode window and its OpenGL context */

	/* Make the window's context current */
	render(window);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}