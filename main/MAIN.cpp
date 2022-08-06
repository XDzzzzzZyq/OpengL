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

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "OpenGL Error: " << error << std::endl;
	}
	else {
		std::cout << "OpenGL has no error " << std::endl;
	}

	if (glewInit() != GLEW_OK) {
		cout << "glew error" << endl;
	}
	else {
		cout << "glew has no error" << endl;
	}

	glfwSetScrollCallback(window, scrollCall);

	Renderer renderer;
	ImguiManager UI;

	Camera camera(10.0f, 10.0f, 70, 0.0f, 300.0f);
	camera.SetPos(glm::vec3(0.0f, 0.0f, 20.0f));
	camera.ApplyTransform();
	camera.GetInvTransform();
	renderer.UseCamera(&camera);
	DEBUG("-------------------------------")

		/* Loop until the user closes the window */

		Mesh go1("res/obj/monkey2.obj");
	go1.SetObjShader("res/shaders/testS.shader");
	go1.SetTex("res/tex/avatar2.png", PNG_TEXTURE);
	go1.SetCenter();
	DEBUG("-------------------------------")

		Mesh go2("res/obj/torus.obj");
	go2.SetObjShader("res/shaders/testS.shader");
	go2.SetTex("res/tex/avatar1.png", PNG_TEXTURE);
	go2.SetCenter();
	go2.SetPos(glm::vec3(4, 0, 0));
	go2.SetScale(glm::vec3(1.5f));
	go2.ApplyTransform();
	DEBUG("-------------------------------")

		renderer.UseMesh(&go1);
	renderer.UseMesh(&go2);

	Light pointLight1(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight1.SetPos(glm::vec3(2.0f, 2.0f, 2.0f));
	pointLight1.ApplyTransform();
	pointLight1.GenFloatData();
	DEBUG("-------------------------------")


		Light pointLight2(POINTLIGHT, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	pointLight2.GenFloatData();
	DEBUG("-------------------------------")

		renderer.UseLight(&pointLight1);
	renderer.UseLight(&pointLight2);


	DebugLine line;
	line.PushDebugLine(5, 5, 5);
	renderer.UseDebugLine(&line);
	//line.is_viewport = false;
	DEBUG("-------------------------------")

		Environment environment("res/tex/hdr/room.hdr");
	renderer.UseEnvironment(&environment);
	DEBUG("-------------------------------")

		DebugPoints points;
	points.PushDebugPoint(5, 5, 5);

	renderer.UseDebugPoints(&points);
	DEBUG("-------------------------------")


		//ImGuiIO& io = ImGui::GetIO(); (void)io;
		//ImGuiStyle& style = ImGui::GetStyle();
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		//io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

		UI.SetConfigFlag(ImGuiConfigFlags_DockingEnable);
	UI.SetConfigFlag(ImGuiConfigFlags_ViewportsEnable);
	UI.SetBackendFlag(ImGuiBackendFlags_PlatformHasViewports);
	UI.SetBackendFlag(ImGuiBackendFlags_PlatformHasViewports);

	if (UI.GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		UI.GetStyle().WindowRounding = 0.0f;
		UI.GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	UI.ManagerInit(window);

	static float scale = 0.3f;
	static float blend = 0.5f;
	static float rotateX = 0.0f;
	static float rotateY = 0.0f;
	static float rotateZ = 0.0f;
	double mouse_x = 0.0f, mouse_y = 0.0f;
	ImVec4 LightColor = ImVec4(1.0f, 0.5f, 0.5f, 1.00f);
	ImVec4 LightPos = ImVec4(0.7f, 0.7f, 1.0f, 1.00f);
	AverageTime<500> AvTime;
	float FrameCount = 0;

	float testfloat[3] = { 0.0f,0.5f,1.0f };

	UI.SetButtonFunc("__Parameters__", "Debug", [&] {
		DEBUG(2)
			glm::vec3 newpoint1 = 8.65f * glm::normalize(glm::vec3(rand11(), rand11(), rand11()));
		points.PushDebugPoint(newpoint1);
		line.PushDebugLine(newpoint1);

		});
	UI.SetButtonFunc("test layer", "testB", [&] {
		DEBUG(1)
			glm::vec3 newpoint2 = 8.65f * glm::normalize(glm::vec3(rand11(), rand11(), rand11()));
		points.PushDebugPoint(newpoint2);
		line.PushDebugLine(newpoint2);

		UI.GetParaValue("test layer", "test")->para_data.fdata = rand11();
		});

	UI.ParaUpdate = [&] {
		FrameCount++;
		UI.FindImguiItem("__Parameters__", "MOUSE_POS : [%.1f : %.1f]")->SetArgsList(2, mouse_x, mouse_y);
		UI.FindImguiItem("__Parameters__", "Application average %.3f ms/frame (%.1f FPS)")->SetArgsList(2, 1000.0f / AvTime.result, AvTime.result);
		UI.FindImguiItem("Viewport", "Viewport")->ResetBufferID(renderer.GetFrameBufferTexture(0));

		scale = UI.GetParaValue("__Parameters__", "SCALE")->para_data.fdata;
		blend = UI.GetParaValue("__Parameters__", "POWER")->para_data.fdata;
		rotateX = UI.GetParaValue("__Parameters__", "X")->para_data.fdata;
		rotateY = UI.GetParaValue("__Parameters__", "Y")->para_data.fdata;
		rotateZ = UI.GetParaValue("__Parameters__", "Z")->para_data.fdata;
	};

	while (!glfwWindowShouldClose(window))
	{
		/* Render here */

		UI.NewFrame();
		AvTime.Add(UI.GetIO().Framerate);
		go1.SetScale(glm::vec3(scale));
		//go1.SetScale(glm::vec3(UI.GetParaValue("test layer", "test")->para_data.fdata));  ////////////  GetData
		go1.SetRot(glm::vec3(0.0f, FrameCount / 50, 0.0f));
		//go1.SetRot(glm::vec3(rotateX, rotateY, rotateZ));

		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		renderer.GetActiveCamera()->EventActivate(window);
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

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		//std::cout << "\r" << timer.duration << "ms";

	}
	cout << endl << "finished" << endl;
	cout << GameObject::count << " object(s)" << endl;
}

int main(void)
{
	std::iostream::sync_with_stdio(false);
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "TEST_WINDOW", NULL, NULL);
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