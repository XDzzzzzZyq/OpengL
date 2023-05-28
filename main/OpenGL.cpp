#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <fstream>
#include <string>
#include <numeric>

#include "support.h"
#include "GameObject.h"
#include "Renderer.h"

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



	/* Loop until the user closes the window */
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	//vertex buffer |   pos2f    |    uv2f    |
	//float data[] = { -0.4f, -0.5f, 0.0f, 0.0f,         //0
	//				  0.4f, -0.5f, 1.0f, 0.0f,         //1
	//				  0.6f,  0.5f, 1.0f, 1.0f,         //2
	//				 -0.6f,  0.5f, 0.0f, 1.0f          //3
	//};
	Reading read = ReadObj("res/obj/sphere.obj", false);
	//float* data = read.vertex.data();
	float* data = read.data_array.data();

	//VertexBuffer vb(data, read.vertex.size()*sizeof(float));
	VertexBuffer vb(data, read.data_array.size()*sizeof(float));
	VertexArray va;

	BufferLayout layout;
	layout.Push<float>(3); //3D position
	layout.Push<float>(2); //UV
	layout.Push<float>(3); //Normal

	va.AddBuffer(vb, layout);
	//verB.Bind();
	/*
	GLuint buff;
	glGenBuffers(1, &buff);
	//cout << buff << endl;
	//cout << sizeof(buff) <<" " << 10 * sizeof(float) << endl;
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
*/


//index buffer
	/*
	GLuint index[] = { 0,1,2,
					   3,4,5 };*/
	const int size = read.count[3]*3;
	std::vector<GLuint>* indexArray=new std::vector<GLuint>(size);
	std::iota(indexArray->begin(), indexArray->end(), 0);
	GLuint* index = indexArray->data();
	IndexBuffer inB(index, indexArray->size()*sizeof(GLuint));
	//inB.Unbind();
	/*
	GLuint indbuff;
	glGenBuffers(1, &indbuff);
	//cout << indbuff << endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indbuff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	*/

	//Shaders shaders("res/shaders/test.shader");
	RenderShader shaders("res/shaders/test.shader");
	shaders.UseShader();
	shaders.SetValue("is", 0.5f);

	glm::mat4 matrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -50.0f, 50.0f);
	shaders.SetValue("U_ProjectM", matrix);
	//Texture tex1("testImg", PNG_TEXTURE, GL_REPEAT);
	Texture tex1("testImg", RGBA_TEXTURE, GL_REPEAT);
	shaders.SetValue("U_Texture", 0);
	tex1.Bind(0);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();

	bool show_demo_window = true;
	bool show_another_window = false;
	static float f = 0.0f;
	static int counter = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		Timer timer;
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glm::vec3 axis = { clear_color.x,clear_color.y,clear_color.z };

		shaders.SetValue("U_color", getrand(), getrand(), getrand(), 1.0f);


		matrix = glm::rotate(matrix, 10 * f, axis);

		shaders.SetValue("U_ProjectM", matrix);
		//shaders.UnuseShader();
		//glDrawArrays(GL_TRIANGLES, 0, 3);
/*		va.Bind();*/
		glDrawElements(GL_TRIANGLES, inB.count(), GL_UNSIGNED_INT, nullptr);
		//renderer.TestRend(va, inB, shaders);


		{


			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		matrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -50.0f, 50.0f);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		//std::cout << "\r" << timer.duration << "ms";

	}

	cout << endl << "finished" << endl;

}

int main(void)
{

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	window = glfwCreateWindow(750, 750, "TEST_WINDOW", NULL, NULL);
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
