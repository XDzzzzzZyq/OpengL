#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "Spirit.h"
#include "Camera.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"

#include "support.h"
#include "structs.h"
#include "Shaders.h"

#include "glm/glm.hpp"

#include <optional>

enum EnvironmentType
{
	NONE_ENVIR, TEXTURE_ENVIR, COLOR_ENVIRN, NOISE_ENVIRN
};

class Environment : public GameObject, public Transform
{
private:
	GLuint framebuffer_id = 0, renderBuffer_id = 0, buff_tex_id = 0;

	std::vector<float> screenQuad = {
		// positions		// texCoords
		-1.0f, 1.0f,	0.0f, 1.0f,
		 1.0f, 1.0f,	1.0f, 1.0f,
		-1.0f,-1.0f,	0.0f, 0.0f,
		 1.0f,-1.0f,	1.0f, 0.0f

	};

	VertexArray o_vertArry;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_indexBuffer;

public:
	std::optional<FrameBuffer> envir_frameBuffer;
	
	Shaders envir_shader;

	Texture frame_buffer, envir_hdr;
	EnvironmentType envir_type = EnvironmentType::NONE_ENVIR;
	bool use_envir = false;

	float envir_power = 0.0f;
	glm::vec3 envir_color = glm::vec3(0.1f);

	Environment(const std::string& texpath);
	Environment();
	~Environment();

	void ChangeEnvirTexture(const std::string& texpath) const;
	void ChangeEnvirType(const EnvironmentType& type) const;

	void BindFrameBuffer() const;
	void UnBindFrameBuffer() const;

	mutable std::vector<float> envir_floatData;
	void GenFloatData() const;

	void RenderEnvironment(const Camera& cam);


};

