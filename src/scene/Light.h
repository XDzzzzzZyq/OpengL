#pragma once

#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "PolygonLight.h"

#include "buffer/FrameBuffer.h"
#include "buffer/StorageBuffer.h"
#include "buffer/UniformBuffer.h"

#include "RenderConfigs.h"

#include <array>

enum LightType
{
	NONELIGHT = -1, POINTLIGHT, SUNLIGHT, SPOTLIGHT, AREALIGHT
};

// A basic light, which can be a point light, sun light, or a spot light
class Light : public GameObject, public Transform3D
{
public:
	bool use_shadow{ true };

	LightType light_type{ LightType::NONELIGHT };
	float light_power{ 1.0f };
	glm::vec3 light_color{ 1.0f }; //3f

	// point light
	float light_radius{ 0.05f };

	// spot light
	float spot_cutoff{ 0.9f };
	float spot_outer_cutoff{ 0.8f };

	// area light
	float area_ratio{ 1.0f };

public:

	Sprite light_sprite;
	Texture light_shadow_map;
	glm::mat4 light_proj{ 1.0f };

public:

	static float sun_shaodow_field;
	static float sun_shaodow_near;
	static float sun_shaodow_far;

	static float point_shaodow_near;
	static float point_shaodow_far;
	static float point_blur_range;

	static float spot_shaodow_near;
	static float spot_shaodow_far;
	static float spot_blur_range;

	static float area_shaodow_near;
	static float area_shaodow_far;
	static float area_blur_range;

public:

	Light();
	Light(LightType type, float power = 10, glm::vec3 color = glm::vec3{ 1, 1, 1 });
	void InitShadowMap(RenderConfigs* config=nullptr);
	inline static std::pair<SpriteType, std::string> ParseLightName(LightType _type);

public:

	bool is_light_changed{ false };
	void SetColor(const glm::vec3& _col);
	void SetPower(float _power);
	void SetShadow(bool _state);
	void SetRadius(float _rad);
	void SetCutoff(float _ang);
	void SetOuterCutoff(float _ang);
	void SetRatio(float _ratio);

private:

	static FrameBuffer _shadowmap_buffer;
	static std::array<ChainedShader, 4> _shadowmap_shader;
	static std::array<glm::mat4, 6> _point_6side;

public:

	static void EnableShadowMap();
	void ConstructSAT();

public:
	void RenderLightSpr(Camera* cam);

public:

	void BindShadowMapBuffer();
	void BindShadowMapShader();
	void BindTargetTrans(const glm::mat4& _trans);
	void UpdateProjMatrix();
	void BindShadowMap(GLuint _slot = GL_TEXTURE31);

public:
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }
};


// Reads a list of lights and converts them into buffers to be passed to the shader
// It parses three basic light types through ParseLightData()
// Polygonal lights are parsed with ParsePolygonLightData()
struct LightArrayBuffer {

public:

	// in GLSL (shader), the size of vec3 equals to vec4 because of alignment
	// https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159

	struct PointStruct
	{
		PointStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::vec3 pos{ 0 };

		alignas(4) float power{ 1.0f };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(4) float radius{ 0.05f };
	};

	struct SunStruct
	{
		SunStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::vec3 pos{ 0 };
		alignas(16) glm::vec3 dir{ 1, 0, 0 };

		alignas(4) float power{ 1.0f };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(16) glm::mat4 proj_trans;
	};

	struct SpotStruct
	{
		SpotStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::vec3 pos{ 0 };
		alignas(16) glm::vec3 dir{ 1, 0, 0 };

		alignas(4) float power{ 1.0f };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(4) float cutoff{ 0.9f };
		alignas(4) float outer_cutoff{ 0.8f };
	};

	struct AreaStruct
	{
		AreaStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };
		alignas(16) glm::mat4 trans{ 1 };

		alignas(4) float power{ 1.0f };
		alignas(4) int use_shadow{ 1 };      // bool -> int
		alignas(4) float ratio{ 1.0f };
	};

	struct PolyStruct
	{
		alignas(16) glm::vec3 color{ 1 };

		alignas(4) float power{ 1.0f };
		alignas(4) int use_shadow{ 1 };
		alignas(4) int n{ 3 };
	};

	struct PolyVertStruct
	{
		alignas(16) glm::vec3 v{ 0, 0, 0 };
	};

	static const GLuint Sizeof_Point = sizeof(PointStruct);
	static const GLuint Sizeof_Sun   = sizeof(SunStruct);
	static const GLuint Sizeof_Spot  = sizeof(SpotStruct);
	static const GLuint Sizeof_Area = sizeof(AreaStruct);
	static const GLuint Sizeof_Poly  = sizeof(PolyStruct);
	static const GLuint Sizeof_PolyVert  = sizeof(PolyVertStruct);

	struct SceneInfo {
		int point_count{ 0 };
		int sun_count{ 0 };
		int spot_count{ 0 };
		int area_count{ 0 };
		int poly_count{ 0 };
		int poly_verts_count{ 0 };
	};

public:

	std::vector<PointStruct> point_list;
	std::vector<SunStruct> sun_list;
	std::vector<SpotStruct> spot_list;
	std::vector<AreaStruct> area_list;
	std::vector<PolyStruct> poly_list;
	std::vector<PolyVertStruct> poly_verts;

	mutable std::unordered_map<int, Texture> shadow_cache;
	StorageBuffer point_buffer, sun_buffer, spot_buffer, area_buffer, poly_buffer, poly_verts_buffer;

	UniformBuffer info;

private:

	using _LightInfo = std::tuple<int, Light*>; // loc & texPtr
	std::unordered_map<int, _LightInfo> light_info_cache;  // id -> loc & texPtr
	GLuint cache_w{};
	GLuint cache_h{};

public:

	LightArrayBuffer();
	~LightArrayBuffer();
	void Init();
	void Bind() const;

public:
	static constexpr int shadow_slot[16]{31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16};
public:

	void ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list);
	void ParsePolygonLightData(const std::unordered_map<int, std::shared_ptr<PolygonLight>>& poly_light_list);

public:

	SceneInfo GetSceneInfo() const;
	GLsizei GetTotalCount() const;
	GLuint GetSlotOffset(LightType _type) const;
	void Resize(GLuint _w, GLuint _h);

public:
	void UpdateLight(Light* light);
	void UpdateLightingCache(int frame, RenderConfigs* config);
	void BindShadowMap() const;
};
