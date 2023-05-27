#pragma once

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

// using singleton
class Application
{
private:
	Application();
	static Application* m_app;
public:
	static Application* Get();
	~Application();

public:
	Renderer renderer{};
	ImguiManager UI{};
	EventListener Event{};
	GLFWwindow* window{ nullptr };

public:
	int Init();
	int Run();
	int Terminate();
};

