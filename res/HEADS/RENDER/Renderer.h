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

#include "EventListener.h"

#include "Spirit.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include <optional>


class Renderer : public EventListener
{
private:
	std::unordered_map<int, Light*> emptyLight;
	std::optional<FrameBuffer> framebuffer;
	std::unordered_map<int, std::string> name_buff;
	std::vector<int> spirit_id_buff;
public:
	Renderer();
	~Renderer();

	//state
public:
	bool is_light_changed = false;

	std::unordered_map<int, GameObject*>		   obj_list; //for selectables
	std::unordered_map<int, Camera*>			   cam_list;
	std::unordered_map<int, Mesh* >			      mesh_list;
	std::unordered_map<int, Light*>				 light_list; //num | pointer
	std::unordered_map<int, Environment*>		 envir_list;
	std::unordered_map<int, Spirit*>	        spirit_list;
	std::unordered_map<int, DebugLine*>	         dLine_list;
	std::unordered_map<int, DebugPoints*>	   dPoints_list;

public:
	bool is_spirit_selected = false;
	bool is_GOlist_changed = true;
	int active_GO_ID;
	std::vector<int> selec_list;
	std::vector<int>parent_index_list;
	OutlineData outline_list;
	OutlineData* GetOutlineData() { return &outline_list; }
	std::vector<int> GetParentRelatData() { return parent_index_list; }
	std::string GetObjectName(int ID);

	int GetSelectID(GLuint x, GLuint y);

public:
	std::vector<FrameBuffer> framebuffer_list;
	int framebuffer_count = -1;
	void AddFrameBuffer();
	void BindFrameBuffer(int slot);
	void EndFrameBuffer(int slot);
	void FrameBufferResize(int slot, const ImVec2& size);
	GLuint GetFrameBufferTexture(int slot);

public:
	bool multi_select = false;
	void EventInit();
	void LMB_CLICK();
	void SHIFT();

public:
	void FrameClean() const;
	void Render();
	void Reset();

public:
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

