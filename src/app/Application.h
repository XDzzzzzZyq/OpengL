#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Input.h"
#include "Context.h"
#include "Controllers.h"
#include "Renderer.h"
#include "ImguiManager.h"

// using singleton
class Application
{
private:
	Application() =default;

public:
	~Application() =default;
	static Application& Get();

public:
	// TODO: separate Editor
	Input InputManager{};
	EventPool EventPool{};
	ControllerManager Controllers{};
	SceneContext Ctx{};
	Renderer renderer{};
	ImguiManager UI{};
	GLFWwindow* window{ nullptr };

public:
	int Init();
	int Run();
	int Terminate();
};

