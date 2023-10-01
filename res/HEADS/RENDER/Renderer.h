#pragma once

#include "EventListener.h"

#include "SceneManager.h"
#include "FrameBuffer.h"
#include "SDFField.h"

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
	GLuint r_frame_num = 0;
	GLuint r_frame_width = SCREEN_W;
	GLuint r_frame_height = SCREEN_H;

private:
	std::shared_ptr<FrameBuffer> r_render_result;
public:
	Renderer();
	void Init();

	~Renderer();
public:

	bool r_using_of = true;				// Optical Flow Correction
	bool r_forward_of = false;
	bool r_deferred_rendering = true;	// Multi-passes Deferred Rendering
	bool r_using_fxaa = true;			// Fast Approximate Anti-Aliasing
	bool r_using_ssr = true;			// Screen Space Reflection
	bool r_using_shadow_map = true;
	bool r_using_SDF_field = true;

	float r_gamma = 1.0f;
	int r_ao_ksize = 16;
	float r_ao_radius = 0.5;

public:

	bool r_render_icons = true;
	bool r_is_preview = true;

public:

	std::shared_ptr<SceneResource> r_scene;

	LightArrayBuffer r_light_data;
	SDFField r_sdf_field;

public:

	std::string GetObjectName(int ID);
	int GetSelectID(GLuint x, GLuint y);

public:

	std::vector<FrameBuffer> r_buffer_list;
	void InitFrameBuffer();
	void BindFrameBuffer(int slot);
	void EndFrameBuffer(int slot);
	void FrameBufferResize(const glm::vec2& size);
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
	void ConstructSDF();

	void Reset();
	void FrameResize(GLuint _w, GLuint _h);

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

