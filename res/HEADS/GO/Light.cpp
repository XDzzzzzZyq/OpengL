#include "Light.h"

Light::Light(LightType type, float power, glm::vec3 color)
{
	o_type = GO_LIGHT;
	light_type = type;
	light_power = power;
	light_color = color;

	light_spirit.spr_type = LIGHT_SPIRIT;
	light_spirit.SetTex();

	o_name = ParseLightName() + std::to_string(GetObjectID());
}

std::string Light::ParseLightName() const
{
	switch (light_type)
	{
	case NONELIGHT:
		return "None Light";
	case POINTLIGHT:
		return "Point Light.";
	case SUNLIGHT: 
		return "Sun.";
	case SPOTLIGHT:
		return "Spot Light.";
	default:
		return "None Light";
	}
}

void Light::SetColor(ImVec4 _col)
{
	if (ImVec4_vec3(_col) == light_color) return;

	is_light_changed = true; 
	light_color = ImVec4_vec3(_col);
}

void Light::SetPower(float _power)
{
	if (_power == light_power) return;

	is_light_changed = true; 
	light_power = _power;
}

void Light::SetShadow(bool _state)
{
	if (_state == use_shadow) return;

	is_light_changed = true; 
	use_shadow = _state;
}

void Light::RenderLightSpr(Camera* cam)
{
	light_spirit.RenderSpirit(vec3_stdVec6(o_position, light_color), cam);
}

void LightFloatArray::Init()
{
	point_buffer = StorageBuffer(CUSTOM_LIST, 0);
	sun_buffer   = StorageBuffer(CUSTOM_LIST, 1);
	spot_buffer  = StorageBuffer(CUSTOM_LIST, 2);
	info = UniformBuffer<SceneInfo>(3);
}

void LightFloatArray::Bind() const
{
	point_buffer.BindBufferBase();
	sun_buffer.BindBufferBase();
	spot_buffer.BindBufferBase();
	info.Bind(0);
}

void LightFloatArray::ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list)
{

	point.clear();
	sun.clear();
	spot.clear();

	for (auto& light : light_list)
	{
		switch (light.second->light_type)
		{
		case NONELIGHT:
			break;
		case POINTLIGHT:
			point.emplace_back(PointStruct{
				light.second->light_color, 0,
				light.second->o_position, 0,
				light.second->light_power,
				(int)light.second->use_shadow,

				light.second->spot_radius
				});
			break;
		case SUNLIGHT:
			sun.emplace_back(SunStruct{
				light.second->light_color, 0,
				light.second->o_position, 0,
				light.second->light_power,
				(int)light.second->use_shadow,

				glm::vec4(light.second->o_rot, 0)
				});
			break;
		case SPOTLIGHT:
			spot.emplace_back(SpotStruct{
				light.second->light_color, 0,
				light.second->o_position, 0,
				light.second->light_power,
				(int)light.second->use_shadow,

				light.second->o_rot, 0,
				light.second->spot_angle,
				1.0f,
				light.second->spot_radius
				});
			break;
		default:
			break;
		}
	}

	point_buffer.GenStorageBuffer(point);
	sun_buffer.GenStorageBuffer(sun);
	spot_buffer.GenStorageBuffer(spot);

	info.Update(GetSceneInfo());
}

LightFloatArray::SceneInfo LightFloatArray::GetSceneInfo() const
{
	SceneInfo info{};

	info.point_count = point.size();
	info.sun_count = sun.size();
	info.spot_count = spot.size();

	LOOP(point.size() + sun.size() + spot.size()) {
		info.shadow_maps[i] = 32 - i - 1;          // binding from 31 -> 0
	}

	return info;
}

GLsizei LightFloatArray::GetTotalCount() const
{
	return point.size() + sun.size() + spot.size();
}