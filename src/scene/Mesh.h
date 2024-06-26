#pragma once

#include "MeshData.h"

#include "Texture.h"
#include "Shaders.h"

#include "Material.h"

class Mesh : public GameObject, public Transform3D
{
public:

	std::shared_ptr<Material> o_material;
	std::shared_ptr<MeshData> o_mesh;
	std::shared_ptr<MeshData> o_mesh_low { nullptr };
	std::shared_ptr<RenderShader> o_shader;

	bool using_shadow{ true };
	bool using_material{ true };
	bool using_sdf{ true };
	bool is_closure{ true };

public:

	Mesh();
	Mesh(const std::string& path);

public:

	void RenderMesh(const Camera* cam);
	void RenderObjProxy(bool using_original = true) const;

public:

	void SetObjShader(std::string vert, std::string frag = "");
	void SetTex(MatParaType _type, std::string _name);
	void SetMatColor(MatParaType _type, float _val);
	void SetMatColor(MatParaType _type, glm::vec3 _col);
	void SetCenter();

	void SetLowPoly(const std::string& path = "");

	void EnableShadow(bool _enable) { using_shadow = _enable; }
	void EnableMaterial(bool _enable) { using_material = _enable; }
	void EnableSDF(bool _enable) { using_sdf = _enable; }

	template<typename... T>
	void SetShaderValue(std::string _name, T ..._v);

public:

	void* GetShader()		override { return o_shader.get(); }
	void* GetMaterial()		override { return o_material.get(); }
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }
};

template<typename... T>
void Mesh::SetShaderValue(std::string _name, T ..._v)
{
	o_shader->UseShader();
	o_shader->SetValue(_name, _v...);
	o_shader->UnuseShader();
}


