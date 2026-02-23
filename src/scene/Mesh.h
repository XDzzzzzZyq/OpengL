/**
 * @file Mesh.h
 * @brief 3D mesh object for renderable geometry in the scene hierarchy.
 * 
 * Mesh is the primary renderable object, representing 3D geometry with material,
 * transform, and shader properties. It inherits from ObjectID (scene identity) and
 * Transform3D (spatial placement). Mesh objects are consumed by the Renderer to
 * produce final images.
 * 
 * Architecture:
 * - Owned by scene graph (shared_ptr in Scene containers)
 * - Renderer reads mesh data, material, and transform as immutable
 * - Editor mutates mesh properties via events and controllers
 * - Supports LOD via optional low-poly proxy mesh
 * 
 * @note GPU resources (buffers, textures) are owned by MeshData and Material, not Mesh.
 * @note Multiple meshes can share the same MeshData and Material via shared_ptr.
 */

#pragma once

#include "Context.h"
#include "ID.h"
#include "Transform.h"

#include "MeshData.h"

#include "Texture.h"
#include "shaders/RenderShader.h"

#include "Material.h"

/**
 * @brief 3D mesh object representing renderable geometry with material and transform.
 * 
 * Mesh combines MeshData (vertex/index buffers), Material (textures and properties),
 * RenderShader (GPU program), and Transform3D (position/rotation/scale) to form a
 * complete renderable object. It supports shadow casting, material shading, and SDF
 * (Signed Distance Field) generation for soft shadows.
 * 
 * Resource Ownership:
 * - MeshData: Shared ownership (multiple meshes can reference same geometry)
 * - Material: Shared ownership (multiple meshes can share materials)
 * - RenderShader: Shared ownership (shaders are reused across meshes)
 * - Transform: Owned directly (each mesh has unique transform)
 * 
 * Rendering Pipeline Integration:
 * - Geometry pass: Uploads mesh data and material to G-Buffer
 * - Shadow pass: Optionally rendered into shadow maps (if using_shadow == true)
 * - SDF pass: Optionally contributes to SDF field (if using_sdf == true)
 * - Proxy rendering: Low-poly mesh can be used for distant objects (LOD)
 * 
 * @note Inheritance: ObjectID for scene identity, Transform3D for spatial transform.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 * @note LOD: Use SetLowPoly() to provide simplified geometry for distance culling.
 */
class Mesh : public ObjectID, public Transform3D
{
public:

	std::shared_ptr<Material> o_material;         ///< Material defining surface properties (albedo, metallic, roughness, etc.)
	std::shared_ptr<MeshData> o_mesh;             ///< High-resolution geometry (vertex/index buffers)
	std::shared_ptr<MeshData> o_mesh_low { nullptr }; ///< Optional low-poly proxy for LOD rendering
	std::shared_ptr<RenderShader> o_shader;       ///< Shader program for rendering this mesh

	bool using_shadow{ true };   ///< Enable shadow casting for this mesh
	bool using_material{ true }; ///< Enable material shading (if false, use flat shading)
	bool using_sdf{ true };      ///< Include this mesh in SDF field generation
	bool is_closure{ true };     ///< Mesh is closed/watertight (affects SDF and culling)

public:

	/**
	 * @brief Constructs a default empty mesh.
	 * @note Mesh has no geometry until o_mesh is assigned.
	 */
	Mesh();
	
	/**
	 * @brief Constructs a mesh from OBJ file.
	 * @param path Path to OBJ file on disk
	 * @note Automatically loads MeshData from file via MeshData constructor.
	 */
	Mesh(const std::string& path);

public:

	/**
	 * @brief Renders the mesh to active framebuffer.
	 * @param ctx Context providing camera, lighting, and render state
	 * @note Binds shader, material, and transform, then issues draw call.
	 * @note Called by Renderer during geometry pass.
	 */
	void RenderMesh(const Context& ctx);
	
	/**
	 * @brief Renders low-poly proxy or original mesh.
	 * @param using_original If true, render high-res mesh; if false, render low-poly proxy
	 * @note Used for LOD rendering or preview modes.
	 */
	void RenderObjProxy(bool using_original = true) const;

public:

	/**
	 * @brief Sets custom shader for this mesh.
	 * @param vert Vertex shader filename (without extension)
	 * @param frag Fragment shader filename (without extension, default: same as vert)
	 * @note Replaces o_shader with newly compiled shader from shader library.
	 */
	void SetObjShader(std::string vert, std::string frag = "");
	
	/**
	 * @brief Assigns texture to material parameter.
	 * @param _type Material parameter type (ALBEDO, METALLIC, ROUGHNESS, NORMAL, etc.)
	 * @param _name Texture filename or path
	 * @note Loads texture and updates material's texture slot.
	 */
	void SetTex(Material::MatParaType _type, std::string _name);
	
	/**
	 * @brief Sets scalar material property.
	 * @param _type Material parameter type (METALLIC, ROUGHNESS, etc.)
	 * @param _val Scalar value (typically 0.0 to 1.0)
	 * @note Updates material's float properties.
	 */
	void SetMatColor(Material::MatParaType _type, float _val);
	
	/**
	 * @brief Sets vector material property.
	 * @param _type Material parameter type (ALBEDO, EMISSION, etc.)
	 * @param _col RGB color vector
	 * @note Updates material's vec3 properties.
	 */
	void SetMatColor(Material::MatParaType _type, glm::vec3 _col);
	
	/**
	 * @brief Centers mesh pivot at geometry centroid.
	 * @note Recomputes bounding box and shifts vertices to center.
	 */
	void SetCenter();

	/**
	 * @brief Assigns low-poly proxy mesh for LOD.
	 * @param path Path to low-poly OBJ file (default: empty, generates automatic simplification)
	 * @note If path is empty, may auto-generate simplified mesh (implementation-dependent).
	 */
	void SetLowPoly(const std::string& path = "");

	/**
	 * @brief Enables or disables shadow casting.
	 * @param _enable True to cast shadows, false to skip shadow pass
	 */
	void EnableShadow(bool _enable) { using_shadow = _enable; }
	
	/**
	 * @brief Enables or disables material shading.
	 * @param _enable True for full PBR, false for flat/debug shading
	 */
	void EnableMaterial(bool _enable) { using_material = _enable; }
	
	/**
	 * @brief Enables or disables SDF contribution.
	 * @param _enable True to include in SDF field, false to exclude
	 * @note SDF is used for soft shadow approximation.
	 */
	void EnableSDF(bool _enable) { using_sdf = _enable; }

	/**
	 * @brief Sets shader uniform value.
	 * @tparam T Variadic parameter pack for uniform types
	 * @param _name Uniform name in shader
	 * @param _v Uniform value(s)
	 * @note Automatically binds/unbinds shader for uniform upload.
	 */
	template<typename... T>
	void SetShaderValue(std::string _name, T ..._v);

public:

	/**
	 * @brief Returns pointer to shader.
	 * @return Pointer to RenderShader as void* (cast required)
	 * @note Overrides ObjectID::GetShader() for polymorphic access.
	 */
	void* GetShader()		override { return o_shader.get(); }
	
	/**
	 * @brief Returns pointer to material.
	 * @return Pointer to Material as void* (cast required)
	 * @note Overrides ObjectID::GetMaterial() for polymorphic access.
	 */
	void* GetMaterial()		override { return o_material.get(); }
	
	/**
	 * @brief Returns pointer to transform.
	 * @return Pointer to Transform3D as void* (cast required)
	 * @note Overrides ObjectID::GetTransform() for polymorphic access.
	 */
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }
};

template<typename... T>
void Mesh::SetShaderValue(std::string _name, T ..._v)
{
	o_shader->UseShader();
	o_shader->SetValue(_name, _v...);
	o_shader->UnuseShader();
}


