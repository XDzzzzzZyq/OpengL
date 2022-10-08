#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "Spirit.h"


#include "glm/glm.hpp"
#include "ImGui/imgui.h"
#include "support.h"

enum LightType
{
	NONELIGHT, POINTLIGHT, SUNLIGHT, SPOTLIGHT
};

class Light : public GameObject, public Transform3D
{
public:

	Spirit light_spirit;

	bool is_shadow = true;

	int light_type = LightType::NONELIGHT;
	float light_power;
	glm::vec3 light_color = glm::vec3(1.0f); //3f

	//SPOTLIGHT
	float spot_angle;
	float spot_radius;

	Light(LightType type, float power, glm::vec3 color);
	void SetColor(const ImVec4& inp) {
		light_color = ImVec4_vec3(inp);
	}

	void EnableShadow(const bool& state) {
		is_shadow = state;
	}

	std::string ParseLightName() const;

	mutable std::vector<float> light_floatData;
	void GenFloatData() const; // 5f + 6f(trans) +2f(spot)
};

struct LightFloatArray {
public:
	std::vector<float> point, sun, spot;
	int point_count=0, sun_count=0, spot_count=0;
	LightFloatArray() {
		point_count = 0;
		sun_count = 0;
		spot_count = 0;
	}

	LightFloatArray(const std::unordered_map<int, Light*>& light_list) {
		ParseLightData(light_list);
	}

	void ParseLightData(const std::unordered_map<int, Light*>& light_list) {
		for (const std::pair<int, Light*>& ele : light_list)
		{
			//ele.second->GenFloatData(); //only happened when paras are changed
			if (ele.second->light_type == 1)
			{
				if (point_count >= 5)
					continue;
				point_count++;
				point.reserve(point.size() + 8);
				LOOP(8) {

					point.push_back(ele.second->light_floatData[i]);
				}
			}
			else if (ele.second->light_type == 2)
			{
				if (sun_count >= 2)
					continue;
				sun_count++;
				sun.reserve(sun.size() + 11);
				LOOP(11) {
					sun.push_back(ele.second->light_floatData[i]);
				}
			}
			else if (ele.second->light_type == 3)
			{
				if (spot_count >= 3)
					continue;
				spot_count++;
				spot.reserve(spot.size() + 13);
				LOOP(13) {
					spot.push_back(ele.second->light_floatData[i]);
				}
			}
		}
		point.insert(point.begin(), point_count);
		sun.insert(sun.begin(), sun_count);
		spot.insert(spot.begin(), spot_count);
	}
};