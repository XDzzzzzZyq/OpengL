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

	// TODO: remove for stateless renderer
	GLuint r_frame_width{};
	GLuint r_frame_height{};

private:

	// TODO: remove for stateless renderer
	std::shared_ptr<FrameBuffer> r_render_result;
	void FrameResize(GLuint _w, GLuint _h);
	void FrameBufferResize(const glm::vec2& size);

public:

	// TODO: remove for stateless renderer
	std::vector<FrameBuffer> r_buffer_list;
	void InitFrameBuffer();
	void BindFrameBuffer(int slot);
	void EndFrameBuffer(int slot);
	FrameBuffer* GetFrameBufferPtr() { return r_render_result.get(); }

public:

	Renderer();
	void Init(EventPool& evt);

	~Renderer();

public:

	// TODO: move to editor layer / RenderContext
	bool r_render_icons = true;
	bool r_is_preview = true;

public:

	// TODO: remove for stateless renderer
	LightArrayBuffer r_light_data;
	RenderConfigs r_config;
	RenderConfigs* GetConfig() { return &r_config; }

public:

	void NewFrame();
	void Render(const Context& ctx, bool rend = true, bool buff = true);

public:

	void Reset();

	void ConstructSDF(const Context& ctx);

public:

	void ScreenShot();

};

