#pragma once

#include "GL/glew.h"

#include "VertexArray.h"
#include "IndexBuffer.h"

#include "Texture.h"
#include "Shaders.h"

#include "GameObject.h"
#include "Camera.h"
#include "Light.h"

#include "Transform.h"
#include <optional>


class Mesh : public GameObject, public Transform3D
{
private:
	Reading read;
	VertexArray o_vertArry;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_index;
	std::optional<Texture> o_tex;

	glm::vec3 center=glm::vec3(0.0f);
public:

	mutable std::optional<RenderShader> o_shader;
	Mesh(const std::string& path);
	Mesh();
	~Mesh();

	void RenderObj(Camera* cam, const std::unordered_map<int, Light*>& light_list);

	void SetObjShader(std::string vert, std::string frag = "");
	void SetTex(std::string path, TextureType slot);
	void SetCenter();
	template<typename T>
	void SetShaderValue(std::string _name, T _v);

	ShaderLib* GetShaderStruct() override { return dynamic_cast<ShaderLib*>(&o_shader.value()); }
	
	void DeleteObj();
};

template<typename T>
void Mesh::SetShaderValue(std::string _name, T _v)
{
	o_shader->UseShader();
	o_shader->SetValue(_name, _v);
	o_shader->UnuseShader();
}

