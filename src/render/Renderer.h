#pragma once

#include "buffer/FrameBuffer.h"
#include "Light.h"

#include "RenderConfigs.h"
#include "Context.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

class Renderer
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
	FrameBuffer* GetFrameBufferPtr() { return r_render_result.get(); }

public:

	Renderer();
	void Init();

	~Renderer();

public:

	bool r_render_icons = true;
	bool r_is_preview = true;

public:

	LightArrayBuffer r_light_data;
	RenderConfigs r_config;
	RenderConfigs* GetConfig() { return &r_config; }

public:

	void NewFrame();
	void Render(const SceneContext& ctx, bool rend = true, bool buff = true);

public:

	void Reset();
	void FrameResize(GLuint _w, GLuint _h);

	void ConstructSDF(const SceneContext& ctx);
	void UpdateLightInfo(const SceneContext & ctx);

public:

	void ScreenShot();

};

