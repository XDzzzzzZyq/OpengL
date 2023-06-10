cmake_minimum_required(VERSION 3.10)
project(OpenGL)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dep/glfw)

add_subdirectory(dep/glm)

find_package(GLEW REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS}
					dep
					res/HEADS
					res/HEADS/GO
					res/HEADS/EXTRA
					res/HEADS/RENDER
					res/HEADS
					res/HEADS/COMPO
					res/HEADS/RENDER/BUFFER
					res/HEADS/UI
                   )

add_executable(OpenGL
			   main/Application.cpp
			   main/MAIN.cpp
			   res/HEADS/UI/ITEM/NodeEditor.cpp
			   res/HEADS/RENDER/BuildinShader.cpp
			   res/HEADS/RENDER/ShaderParser.cpp
			   res/HEADS/UI/ITEM/OpaButton.cpp
			   res/HEADS/UI/ImguiTheme.cpp
			   res/HEADS/UI/ITEM/TextEditor.cpp
			   res/HEADS/UI/LAYER/ParamControl.cpp
			   res/HEADS/UI/LAYER/Outliner.cpp
			   res/HEADS/UI/ITEM/Button.cpp
			   res/HEADS/UI/ITEM/ParaInput.cpp
			   res/HEADS/UI/DefultViewports.cpp
			   res/HEADS/UI/ImguiMenuItem.cpp
			   res/HEADS/UI/ImguiMenu.cpp
			   res/HEADS/UI/ImguiItem.cpp
			   res/HEADS/UI/Nodes.cpp
			   res/HEADS/COMPO/Parameters.cpp
			   res/HEADS/UI/ImguiLayer.cpp
			   res/HEADS/UI/ImguiManager.cpp
			   res/HEADS/RENDER/BUFFER/FrameBuffer.cpp
			   res/HEADS/RENDER/BUFFER/RenderBuffer.cpp
			   res/HEADS/COMPO/Field.cpp
			   res/HEADS/COMPO/PostProcessing.cpp
			   res/HEADS/GO/Empty.cpp
			   res/HEADS/RENDER/Material.cpp
			   res/HEADS/GO/DebugMesh.cpp
			   res/HEADS/COMPO/Transform.cpp
			   res/HEADS/GO/Environment.cpp
			   res/HEADS/GO/DebugLine.cpp
			   res/HEADS/GO/DebugPoints.cpp
			   res/HEADS/COMPO/EventListener.cpp
			   res/HEADS/GO/ImagePlane.cpp
			   res/HEADS/GO/Camera.cpp
			   dep/ImGui/imgui.cpp
			   dep/ImGui/imgui_demo.cpp
			   dep/ImGui/imgui_draw.cpp
			   dep/ImGui/imgui_impl_glfw.cpp
			   dep/ImGui/imgui_impl_opengl3.cpp
			   dep/ImGui/imgui_tables.cpp
			   dep/ImGui/imgui_widgets.cpp
			   res/HEADS/GO/Light.cpp
			   res/HEADS/GO/GameObject.cpp
			   res/HEADS/RENDER/Renderer.cpp
			   res/HEADS/GO/Mesh.cpp
			   res/HEADS/RENDER/ImGuiWIndow.cpp
			   res/HEADS/RENDER/BufferLayout.cpp
			   res/HEADS/RENDER/BUFFER/IndexBuffer.cpp
			   res/HEADS/stb_image/stb_image.cpp
			   res/HEADS/RENDER/BUFFER/VertexBuffer.cpp
			   res/HEADS/RENDER/VertexArray.cpp
			   res/HEADS/RENDER/Shaders.cpp
			   res/HEADS/RENDER/Texture.cpp
			   res/HEADS/GO/Spirit.cpp
			   res/HEADS/RENDER/BUFFER/StorageBuffer.cpp
			   res/HEADS/RENDER/BUFFER/UniformBuffer.cpp
			   res/HEADS/UI/ITEM/TextureView.cpp
			   res/HEADS/UI/ITEM/Text.cpp
			   res/HEADS/UI/LAYER/Viewport.cpp
			   res/HEADS/RENDER/ShaderLib.cpp
			   res/HEADS/UI/LAYER/ShaderEditor.cpp
			   res/HEADS/UI/NodesColor.cpp
              )

target_link_libraries(OpenGL ${GLEW_LIBRARIES} glfw OpenGL::GL glm::glm)

if(MSVC)
    target_compile_options(OpenGL PRIVATE /std:c++20 /W4)
else()
    target_compile_options(OpenGL PRIVATE -std=c++20 -Wall -Wextra -pedantic)
endif()
