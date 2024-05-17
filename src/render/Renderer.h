#pragma once

#include "EventListener.h"

#include "SceneManager.h"
#include "buffer/FrameBuffer.h"
#include "SDFField.h"
#include "RenderConfigs.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

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

	GLuint r_frame_width{};
	GLuint r_frame_height{};

private:

	std::shared_ptr<FrameBuffer> r_render_result;

public:

	std::vector<FrameBuffer> r_buffer_list;
	void InitFrameBuffer();
	void BindFrameBuffer(int slot);
	void EndFrameBuffer(int slot);
	void FrameBufferResize(const glm::vec2& size);
	GLuint GetFrameBufferTexture(int slot);

public:

	Renderer();
	void Init();

	~Renderer();

public:

	bool r_render_icons = true;
	bool r_is_preview = true;

public:

	std::shared_ptr<SceneResource> r_scene;

	LightArrayBuffer r_light_data;
	RenderConfigs r_config;
	RenderConfigs* GetConfig() { return &r_config; }

public:

	std::string GetObjectName(int ID);
	int GetSelectID(GLuint x, GLuint y);

public:

	bool multi_select = false;
	void EventInit();
	void LMB_CLICK();
	void SHIFT();

public:

	void NewFrame();

	void Render(bool rend = true, bool buff = true);
	void RenderShadowMap(Light* light);
	void ConstructSDF();

public:

	void Reset();
	void FrameResize(GLuint _w, GLuint _h);

	void OnRenderCfgUpdate(RenderConfigs::ModifyFlags flag);
	void UpdateLightInfo();

public:

	void UseScene(std::shared_ptr<SceneResource> _scene);

	void ActivateCamera(int cam_id);
	void ActivateEnvironment(int envir_id);

	std::shared_ptr<Camera> GetActiveCamera();
	std::shared_ptr<Environment> GetActiveEnvironment();
	std::shared_ptr<PostProcessing> GetPPS(int _tar);

public:

	void ScreenShot();

};

