#pragma once

#include "support.h"

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Environment.h"
#include "DebugLine.h"
#include "DebugPoints.h"
#include "PostProcessing.h"
#include "FrameBuffer.h"

#include "Spirit.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include <optional>


class Renderer
{
private:
	std::unordered_map<int, Light*> emptyLight;
	std::optional<FrameBuffer> framebuffer;
public:
	Renderer();
	~Renderer();

	//state
	bool is_light_changed = false;

	std::unordered_map<int, Camera*>		   cam_list;
	std::unordered_map<int, Mesh* >		      mesh_list;
	std::unordered_map<int, Light*>			 light_list; //num | pointer
	std::unordered_map<int, Environment*>	 envir_list;
	std::unordered_map<int, Spirit*>        spirit_list;
	std::unordered_map<int, DebugLine*>      dLine_list;
	std::unordered_map<int, DebugPoints*>  dPoints_list;

	//std::vector<GameObject> selec_list;
	//std::vector<std::tuple<int, std::string>> GetOutlineData();

	std::vector<FrameBuffer> framebuffer_list;
	int framebuffer_count = -1;
	void AddFrameBuffer();
	void BindFrameBuffer(int slot);
	void EndFrameBuffer(int slot);
	void FrameBufferResize(int slot, const ImVec2& size);
	GLuint GetFrameBufferTexture(int slot);

	void FrameClean() const;
	void Render();

	bool is_GOlist_changed = true;
	void UseCamera(Camera* camera);
	void UseCamera(const int& cam_id);
	Camera* GetActiveCamera();

	void UseMesh(Mesh* mesh);

	void UseLight(Light* light);

	void UseEnvironment(Environment* envir);
	void UseEnvironment(const int& envir_id);
	Environment* GetActiveEnvironment();

	void UseDebugLine(DebugLine* dline);
	void UseDebugPoints(DebugPoints* dpoints);
};

