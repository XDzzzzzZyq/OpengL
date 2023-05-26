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
	enum _BuildinPPS
	{
		_PBR_COMP_PPS, _EDIT_VISUAL_PPS
	};

	enum _BuildinBuffLoc
	{
		_RASTER
	};

private:
	std::optional<FrameBuffer> r_render_result;
	std::unordered_map<int, std::string> name_buff;
	std::vector<int> spirit_id_buff;
public:
	Renderer();
	void Init();

	~Renderer();
public:
	bool r_deferred_rendering = true;
	bool r_using_fxaa = true;

	float r_gamma = 1.0f;
public:
	bool is_light_changed = false;

	std::unordered_map<int, std::shared_ptr<GameObject>>		obj_list; //for selectables
	std::unordered_map<int, std::shared_ptr<Camera>>			cam_list;
	std::unordered_map<int, std::shared_ptr<Mesh>>		       mesh_list;
	std::unordered_map<int, std::shared_ptr<Light>>			  light_list; //num | pointer
	std::unordered_map<int, std::shared_ptr<Environment>>	  envir_list;
	std::unordered_map<int, std::shared_ptr<Spirit>>	     spirit_list;
	std::unordered_map<int, std::shared_ptr<DebugLine>>		  dLine_list;
	std::unordered_map<int, std::shared_ptr<DebugPoints>>	dPoints_list;
	std::vector		  <std::shared_ptr<PostProcessing>>			pps_list;

public:
	std::string GetObjectName(int ID);

	int GetSelectID(GLuint x, GLuint y);

public:
	std::vector<FrameBuffer> r_buffer_list;
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
	void UpdateFrame();
	void Render(bool rend = true, bool buff = true);
	void Reset();

public:
	void UseCamera(std::shared_ptr<Camera> camera);
	void UseCamera(const int& cam_id);
	std::shared_ptr<Camera> GetActiveCamera();

	void UseMesh(std::shared_ptr<Mesh> mesh);

	void UseLight(std::shared_ptr<Light>light);

	void UseEnvironment(std::shared_ptr<Environment> envir);
	void UseEnvironment(const int& envir_id);
	std::shared_ptr<Environment> GetActiveEnvironment();

	void UseDebugLine(std::shared_ptr<DebugLine> dline);
	void UseDebugPoints(std::shared_ptr<DebugPoints> dpoints);

	void UsePostProcessing(std::shared_ptr<PostProcessing> pps);

public:
	std::shared_ptr<PostProcessing> GetPPS(int _tar) { return pps_list[_tar]; }

};

