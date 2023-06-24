#pragma once

#include "support.h"

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "PolygonLight.h"
#include "Environment.h"
#include "DebugLine.h"
#include "DebugPoints.h"
#include "PostProcessing.h"
#include "FrameBuffer.h"

#include "EventListener.h"

#include "Sprite.h"

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
		_RASTER, _AO_ELS
	};

public:
	static GLint max_resolution_w;
	static GLint max_resolution_h;

public:
	GLuint r_frame_num = 0;
	GLuint r_frame_width = SCREEN_W;
	GLuint r_frame_height = SCREEN_H;

private:
	std::optional<FrameBuffer> r_render_result;
	std::unordered_map<int, std::string> name_buff;
	std::vector<int> sprite_id_buff;
public:
	Renderer();
	void Init();

	~Renderer();
public:
	bool r_deferred_rendering = true;
	bool r_using_fxaa = true;

	float r_gamma = 1.0f;
	int r_ao_ksize = 16;
public:
	bool is_light_changed = false;

	std::unordered_map<int, std::shared_ptr<GameObject>>		obj_list; //for selectables
	std::unordered_map<int, std::shared_ptr<Camera>>			cam_list;
	std::unordered_map<int, std::shared_ptr<Mesh>>		       mesh_list;
	std::unordered_map<int, std::shared_ptr<Light>>			  light_list; //num | pointer
	std::unordered_map<int, std::shared_ptr<PolygonLight>>  poly_light_list;
	std::unordered_map<int, std::shared_ptr<Environment>>	  envir_list;
	std::unordered_map<int, std::shared_ptr<Sprite>>	     sprite_list;
	std::unordered_map<int, std::shared_ptr<DebugLine>>		  dLine_list;
	std::unordered_map<int, std::shared_ptr<DebugPoints>>	dPoints_list;
	std::vector		  <std::shared_ptr<PostProcessing>>			pps_list;

	LightArrayBuffer r_light_data;

public:

	std::string GetObjectName(int ID);
	int GetSelectID(GLuint x, GLuint y);

public:

	std::vector<FrameBuffer> r_buffer_list;
	void InitFrameBuffer();
	void BindFrameBuffer(int slot);
	void EndFrameBuffer(int slot);
	void FrameBufferResize(const ImVec2& size);
	GLuint GetFrameBufferTexture(int slot);

public:

	bool multi_select = false;
	void EventInit();
	void LMB_CLICK();
	void SHIFT();

public:

	void NewFrame();

	void Render(bool rend = true, bool buff = true);
	void RenderShadowMap(Light* light);

	void Reset();
	void FrameResize(GLuint _w, GLuint _h);

public:

	void UseCamera(std::shared_ptr<Camera> camera);
	void UseCamera(const int& cam_id);
	std::shared_ptr<Camera> GetActiveCamera();

	void UseMesh(std::shared_ptr<Mesh> mesh);

	void UseLight(std::shared_ptr<Light> light);
	void UsePolygonLight(std::shared_ptr<PolygonLight> polyLight);

	void UseEnvironment(std::shared_ptr<Environment> envir);
	void UseEnvironment(const int& envir_id);
	std::shared_ptr<Environment> GetActiveEnvironment();

	void UseDebugLine(std::shared_ptr<DebugLine> dline);
	void UseDebugPoints(std::shared_ptr<DebugPoints> dpoints);

	void UsePostProcessing(std::shared_ptr<PostProcessing> pps);
	std::shared_ptr<PostProcessing> GetPPS(int _tar);

};

