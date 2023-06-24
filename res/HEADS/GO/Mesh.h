#pragma once

#include "MeshData.h"

class Mesh : public GameObject, public Transform3D
{
private:
	std::optional<Texture> o_tex;
public:

	std::shared_ptr<MeshData> o_mesh;
	mutable std::optional<RenderShader> o_shader;

	bool using_shadow{ true };

public:

	Mesh(const std::string& path);
	Mesh();
	~Mesh();

public:

	void RenderObj(Camera* cam);
	void RenderObjProxy() const;

public:

	void SetObjShader(std::string vert, std::string frag = "");
	void SetTex(std::string path, TextureType slot);
	void SetCenter();
	void SetShadow(bool _shadow);
	template<typename... T>
	void SetShaderValue(std::string _name, T ..._v);

public:

	ShaderLib* GetShaderStruct() override { return dynamic_cast<ShaderLib*>(&o_shader.value()); }
	
	void DeleteObj();
};

template<typename... T>
void Mesh::SetShaderValue(std::string _name, T ..._v)
{
	o_shader->UseShader();
	o_shader->SetValue(_name, _v...);
	o_shader->UnuseShader();
}


