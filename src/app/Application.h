#pragma once

#include <memory>

#include "Input.h"
#include "Context.h"
#include "Controllers.h"
#include "Window.h"
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
	Window window;                      ///< GLFW window and OpenGL context owner
	Context Ctx;                        ///< Event-subscribed context (depends on EventPool)
	std::unique_ptr<Renderer> renderer; ///< Renderer (constructed after window)
	std::unique_ptr<ImguiManager> UI;   ///< UI (constructed after window and renderer)

public:
	int Run();
};

