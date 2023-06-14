#pragma once

#include "support.h"

#include "GameObject.h"
#include "Transform.h"
#include "Spirit.h"
#include "AreaLight.h"

#include "StorageBuffer.h"
#include "UniformBuffer.h"

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

	Spirit light_spirit;

public:

	Light(LightType type, float power = 10, glm::vec3 color = glm::vec3{ 1, 1, 1 });
	inline static std::pair<SpiritType, std::string> ParseLightName(LightType _type);

public:

	bool is_light_changed{ false };
	void SetColor(ImVec4 _col);
	void SetPower(float _power);
	void SetShadow(bool _state);
	void SetRadius(float _rad);
	void SetCutoff(float _ang);
	void SetOuterCutoff(float _ang);

public:
	void RenderLightSpr(Camera* cam);
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
		GLuint shadow_maps[32];
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
	std::unordered_map<int, int> id_loc_cache;

public:
	LightArrayBuffer() {};
	~LightArrayBuffer();
	void Init();
	void Bind() const;

	void ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list);
	void ParseAreaLightData(const std::unordered_map<int, std::shared_ptr<AreaLight>>& area_light_list);
	SceneInfo GetSceneInfo() const;
	inline GLsizei GetTotalCount() const;

	void UpdateLight(const std::pair<int, std::shared_ptr<Light>>& light);
};
