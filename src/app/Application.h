#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "ImguiManager.h"
#include "Event.h"
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
	EventCallback Event{};
	EventPool EventPool{};
	GLFWwindow* window{ nullptr };

public:
	int Init();
	int Run();
	int Terminate();
};

