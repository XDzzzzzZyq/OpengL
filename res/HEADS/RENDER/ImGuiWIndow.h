#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "GL/glew.h"

//[ʹ�õ���ģʽ]

class ImGuiWIndow
{
private:
	ImGuiWIndow(GLFWwindow* window);
	static ImGuiWIndow m_window;
public:
	void StartFrame();
	void Render();
	void ShutDown();
	void SetStyle();
	static ImGuiWIndow Get() {
		return m_window;
	};
};

