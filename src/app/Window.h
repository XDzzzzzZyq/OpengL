/**
 * @file Window.h
 * @brief RAII wrapper for GLFW initialization and window/context lifecycle.
 *
 * Window encapsulates the GLFW library init, OS window creation, and
 * OpenGL context activation. Constructing a Window object guarantees
 * steps 1–3 of the required initialization order:
 *
 *   1. glfwInit()
 *   2. glfwCreateWindow()
 *   3. glfwMakeContextCurrent()
 *
 * Components that require an active OpenGL context (e.g., Renderer,
 * ImguiManager) must take Window& in their constructor to statically
 * enforce that the context is ready before they initialize.
 *
 * @note Thread-safety: Must be created and used from the main thread only.
 */

#pragma once

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

/**
 * @brief RAII owner of the GLFW library, OS window, and OpenGL context.
 *
 * Initializes GLFW on construction and terminates it on destruction.
 * After a successful construction the OpenGL 4.6 core-profile context is
 * current on the calling thread and ready for use by Renderer and ImguiManager.
 *
 * @note Non-copyable and non-movable.
 */
class Window
{
public:
	/**
	 * @brief Initializes GLFW and creates an OpenGL 4.6 core-profile window.
	 *
	 * Enforces the required GLFW/GL initialization sequence:
	 *  1. glfwInit()
	 *  2. glfwCreateWindow()
	 *  3. glfwMakeContextCurrent()
	 *
	 * @param width  Window width in pixels
	 * @param height Window height in pixels
	 * @param title  Window title string
	 * @throws std::runtime_error on GLFW init or window creation failure
	 */
	Window(int width, int height, const std::string& title);

	/**
	 * @brief Destroys the GLFW window and terminates the GLFW library.
	 */
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	/**
	 * @brief Returns the native GLFW window handle.
	 * @return Non-owning pointer to GLFWwindow
	 */
	GLFWwindow* Get() const { return handle; }

	/**
	 * @brief Returns true if the window has received a close request.
	 */
	bool ShouldClose() const { return glfwWindowShouldClose(handle); }

	/**
	 * @brief Swaps front and back buffers.
	 */
	void SwapBuffers() const { glfwSwapBuffers(handle); }

private:
	bool glfw_initialized{ false }; ///< True after successful glfwInit(); guards glfwTerminate() in destructor
	GLFWwindow* handle{ nullptr };  ///< Native GLFW window handle
};
