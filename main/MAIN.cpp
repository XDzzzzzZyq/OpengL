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
#include "Environment.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

using std::cout;
using std::endl;
using std::string;



void render(GLFWwindow* window) {
	glfwMakeContextCurrent(window);

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
	Camera camera(10.0f, 10.0f, 70, 0.0f, 300.0f);
	camera.SetPos(glm::vec3(0.0f, 0.0f, 20.0f));
	camera.ApplyTransform();
	camera.GetInvTransform();

	renderer.UseCamera(&camera);
	
	/* Loop until the user closes the window */

	Mesh go1("res/obj/monkey2.obj");
	go1.SetObjShader("res/shaders/testS.shader");
	go1.SetTex("res/tex/avatar2.png", IMAGE_TEXTURE);
	go1.SetCenter();

	Mesh go2("res/obj/torus.obj");
	go2.SetObjShader("res/shaders/testS.shader");
	go2.SetTex("res/tex/avatar1.png", IMAGE_TEXTURE);
	go2.SetCenter();
	go2.SetPos(glm::vec3(4, 0, 0));
	go2.SetScale(glm::vec3(1.5f));
	go2.ApplyTransform();
	
	renderer.UseMesh(&go1);
	renderer.UseMesh(&go2);
	
	Light pointLight1(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight1.SetPos(glm::vec3(2.0f, 2.0f, 2.0f));
	pointLight1.ApplyTransform();
	pointLight1.GenFloatData();

	Light pointLight2(POINTLIGHT, 1.0f, glm::vec3(1.0f,1.0f,1.0f));
	pointLight2.GenFloatData();

	renderer.UseLight(&pointLight1);
	renderer.UseLight(&pointLight2);

	DebugLine line(glm::vec3(-10, -10, 0), glm::vec3(10, 10, 0));
	renderer.UseDebugLine(&line);
	
	Environment environment("res/tex/hdr/room.hdr");
	renderer.UseEnvironment(&environment);
	
	
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();

	static float scale = 0.3f;
	static float blend = 0.5f;
	static float rotateX = 0.0f;
	static float rotateY = 0.0f;
	static float rotateZ = 0.0f;
	double mouse_x, mouse_y = 0.0f;
	ImVec4 LightColor = ImVec4(1.0f, 0.5f, 0.5f, 1.00f);
	ImVec4 LightPos = ImVec4(0.7f, 0.7f, 1.0f, 1.00f);
	AverageTime<500> AvTime;
	float FrameCount = 0;
	
	float testfloat[3] = { 0.0f,0.5f,1.0f };

	while (!glfwWindowShouldClose(window))
	{
		
		FrameCount++;
		/* Render here */
		
		//environment.BindFrameBuffer();
		

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		AvTime.Add(ImGui::GetIO().Framerate);

		
		go1.SetScale(glm::vec3(scale));
		go1.SetRot(glm::vec3(0.0f, FrameCount / 50, 0.0f));
		//go1.SetRot(glm::vec3(rotateX, rotateY, rotateZ));
		
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		renderer.GetActiveCamera()->CameraEventActivate(window);
		
		go1.o_shader.SetValue("z_inp", 100*(blend-0.5f));
		go1.o_shader.SetValue("blen",3, &testfloat[0]);
		
		pointLight1.SetColor(LightColor);
		pointLight1.SetPos(ImVec4_vec3_Uni(LightPos,10.0f));
		pointLight1.light_power = blend * 50;
		pointLight1.GenFloatData();

		pointLight2.SetPos(ImVec4_vec3_Uni(LightPos, -10.0f));
		pointLight2.light_power = blend * 20;
		pointLight2.GenFloatData();

		renderer.is_light_changed = true;

		line.SetPos(glm::vec3(rotateX,0,0));
		line.dLine_color = glm::vec3(1, (90-rotateY)/90, (90 - rotateZ) / 90);
		


		renderer.Render();


		{
			ImGui::BeginMainMenuBar();
/*			ImGui::BeginMenuBar();*/
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
/*			ImGui::EndMenuBar();*/
			ImGui::EndMainMenuBar();
			ImGui::Begin("__Parameters__");                     
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / AvTime.result, AvTime.result/*ImGui::GetIO().Framerate*/);
			ImGui::Text("MOUSE_POS : [%.1f : %.1f]",mouse_x,mouse_y);
			ImGui::SliderFloat("SCALE", &scale, 0.0f, 1.0f);          
			ImGui::SliderFloat("POWER", &blend, 0.0f, 1.0f);
			ImGui::SliderFloat("X", &rotateX, -90.0f, 90.0f);
			ImGui::SliderFloat("Y", &rotateY, -90.0f, 90.0f);
			ImGui::SliderFloat("Z", &rotateZ, -90.0f, 90.0f);
			ImGui::ColorEdit3("Light Color", (float*)&LightColor); 
			ImGui::ColorEdit3("Light Position", (float*)&LightPos);

			if (ImGui::Button("Debug"))
			{
				//std::cout << renderer.GetActiveCamera()->cam_frustum<<"\n";
// 				DEBUG(renderer.GetActiveCamera()->is_TransF_changed)
// 				DEBUG(renderer.GetActiveCamera()->is_TransF_changed)
				rotateX = rotateY = rotateZ = 0.0f;
				line.using_stipple = !line.using_stipple;
			}

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		//std::cout << "\r" << timer.duration << "ms";

	}
	cout << endl << "finished" << endl;
	cout << GameObject::count <<" object(s)" << endl;
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