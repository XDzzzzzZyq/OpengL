#pragma once

#include "support.h"

#include "GameObject.h"
#include "Transform.h"
#include "Spirit.h"
#include "AreaLight.h"
#include "FrameBuffer.h"

#include "StorageBuffer.h"
#include "UniformBuffer.h"

#include <array>

enum LightType
{
	NONELIGHT, POINTLIGHT, SUNLIGHT, SPOTLIGHT
};

// A basic light, which can be a point light, sun light, or a spot light
class Light : public GameObject, public Transform3D
{
public:
	bool use_shadow{ true };

	LightType light_type{ LightType::NONELIGHT };
	float light_power{ 1 };
	glm::vec3 light_color{ 1. }; //3f

	// point light
	float light_radius{ 5 };

	// spot light
	float spot_cutoff{ 0.9 };
	float spot_outer_cutoff{ 0.8 };

public:

	Spirit light_spirit;
	Texture light_shadow_map;
	glm::mat4 light_proj{ 1 };

public:

	static float sun_shaodow_field;
	static float sun_shaodow_near;
	static float sun_shaodow_far;

	static float point_shaodow_near;
	static float point_shaodow_far;

public:

	Light();
	Light(LightType type, float power = 10, glm::vec3 color = glm::vec3{ 1, 1, 1 });
	void InitShadowMap();
	inline static std::pair<SpiritType, std::string> ParseLightName(LightType _type);

public:

	bool is_light_changed{ false };
	void SetColor(ImVec4 _col);
	void SetPower(float _power);
	void SetShadow(bool _state);
	void SetRadius(float _rad);
	void SetCutoff(float _ang);
	void SetOuterCutoff(float _ang);


private:
	static FrameBuffer _shadowmap_buffer;
	static FastLoadShader _shadowmap_shader;
	static ChainedShader _pointshadow_shader;
	static std::array<glm::mat4, 6> _point_6side;

public:
	void RenderLightSpr(Camera* cam);

	void BindShadowMapBuffer();
	void BindShadowMapShader();
	void BindTargetTrans(const glm::mat4& _trans);
	void UpdateProjMatrix();
	void BindShadowMap(GLuint _slot = GL_TEXTURE31);
};


// Reads a list of lights and converts them into buffers to be passed to the shader
// It parses three basic light types through ParseLightData()
// Area lights are parsed with ParseAreaLightData()
struct LightArrayBuffer {

public:

	// in GLSL (shader), the size of vec3 equals to vec4 because of alignment
	// https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159

	struct PointStruct
	{
		PointStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::vec3 pos{ 0 };

		alignas(4) float power{ 1 };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(4) float radius{ 5 };
	};

	struct SunStruct
	{
		SunStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::vec3 pos{ 0 };
		alignas(16) glm::vec3 dir{ 1, 0, 0 };

		alignas(4) float power{ 1 };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(64) glm::mat4 proj_trans;
	};

	struct SpotStruct
	{
		SpotStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::vec3 pos{ 0 };
		alignas(16) glm::vec3 dir{ 1, 0, 0 };

		alignas(4) float power{ 1 };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(4) float cutoff{ 0.9 };
		alignas(4) float outer_cutoff{ 0.8 };
	};

	struct AreaStruct
	{
		alignas(16) glm::vec3 color{ 1 };

		alignas(4) float power{ 1 };
		alignas(4) int use_shadow{ 1 };
		alignas(4) int n{ 3 };
	};

	struct AreaVertStruct
	{
		alignas(16) glm::vec3 v{ 0, 0, 0 };
	};

	static const GLuint Sizeof_Point = sizeof(PointStruct);
	static const GLuint Sizeof_Sun   = sizeof(SunStruct);
	static const GLuint Sizeof_Spot  = sizeof(SpotStruct);
	static const GLuint Sizeof_Area  = sizeof(AreaStruct);
	static const GLuint Sizeof_AreaVert  = sizeof(AreaVertStruct);

	struct SceneInfo {
		int point_count{ 0 };
		int sun_count{ 0 };
		int spot_count{ 0 };
		int area_count{ 0 };
		int area_verts_count{ 0 };
	};

public:
	std::vector<PointStruct> point;
	std::vector<SunStruct> sun;
	std::vector<SpotStruct> spot;
	std::vector<AreaStruct> area;
	std::vector<AreaVertStruct> area_verts;
	StorageBuffer point_buffer, sun_buffer, spot_buffer, area_buffer, area_verts_buffer;
	UniformBuffer<SceneInfo> info;

private:
	using LightInfo = std::tuple<int, LightType, GLuint>;
	std::unordered_map<int, LightInfo> light_info_cache; // id -> loc & map_tex_ID

public:
	LightArrayBuffer() {};
	~LightArrayBuffer();
	void Init();
	void Bind() const;

	void ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list);
	void ParseAreaLightData(const std::unordered_map<int, std::shared_ptr<AreaLight>>& area_light_list);

	SceneInfo GetSceneInfo() const;
	inline GLsizei GetTotalCount() const;
	GLuint GetSlotOffset(LightType _type) const;

	void UpdateLight(const std::pair<int, std::shared_ptr<Light>>& light);
	void BindShadowMap() const;
};
