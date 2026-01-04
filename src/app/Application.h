#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "ImguiManager.h"
#include "EventListener.h"
#include "Context.h"

// using singleton
class Application
{
private:
	Application() =default;

public:
	~Application() =default;
	static Application& Get();

public:
	Renderer renderer{};
	ImguiManager UI{};
	SceneContext Ctx{};
	EventListener Event{};
	GLFWwindow* window{ nullptr };

public:
	int Init();
	int Run();
	int Terminate();
};

