#pragma once

#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Input.h"
#include "Context.h"
#include "Controllers.h"
#include "Renderer.h"
#include "ImguiManager.h"

class Application
{
public:
	Application();
	~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

public:
	// TODO: separate Editor
	Input InputManager{};
	EventPool EventPool{};
	ControllerManager Controllers{};
	Context Ctx;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<ImguiManager> UI;
	GLFWwindow* window{ nullptr };

public:
	int Run();
};

