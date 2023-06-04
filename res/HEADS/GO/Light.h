#pragma once

#include "support.h"

#include "GameObject.h"
#include "Transform.h"
#include "Spirit.h"
#include "StorageBuffer.h"

enum LightType
{
	NONELIGHT, POINTLIGHT, SUNLIGHT, SPOTLIGHT
};

class Light : public GameObject, public Transform3D
{
public:
	bool use_shadow{ true };

	LightType light_type{ LightType::NONELIGHT };
	float light_power{ 1 };
	glm::vec3 light_color{ 1. }; //3f

	//SPOTLIGHT
	float spot_angle{ 30 };
	float spot_radius{ 5 };

	Spirit light_spirit;

public:

	Light(LightType type, float power = 10, glm::vec3 color = glm::vec3{1, 1, 1});
	std::string ParseLightName() const;

public:
	bool is_light_changed{ false };
	void SetColor(ImVec4 _col);
	void SetPower(float _power);
	void SetShadow(bool _state);

public:
	void RenderLightSpr(Camera* cam);
};


struct LightFloatArray {

public:

	struct BasicStruct
	{
		// in GLSL (shader), the size of vec3 equals to vec4 because of alignment
		glm::vec3 color{ 1 }; float offset_0;
		glm::vec3 pos  { 0 }; float offset_1;
		float strength { 1 };
		int use_shadow { 1 };      // bool -> int
		GLuint U_shadow{ 0 };
	};

	struct PointStruct : public BasicStruct
	{
		float radius{ 5 };
	};

	struct SunStruct : public BasicStruct
	{
		glm::vec3 dir{ 1, 0, 0 }; float offset_2;
	};

	struct SpotStruct : public BasicStruct
	{
		glm::vec3 dir{ 1, 0, 0 }; float offset_0;
		float angle{ 30 };
		float ratio{ .1 };
		float fall_off{ .5 };
	};

	static const GLuint Sizeof_Point = sizeof(PointStruct);
	static const GLuint Sizeof_Sun	 = sizeof(SunStruct);
	static const GLuint Sizeof_Spot  = sizeof(SpotStruct);

public:
	std::vector<PointStruct> point;
	std::vector<SunStruct> sun;
	std::vector<SpotStruct> spot;
	StorageBuffer point_buffer, sun_buffer, spot_buffer;

public:
	LightFloatArray() {};
	void Init();
	void Bind() const;

	void ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list);
};