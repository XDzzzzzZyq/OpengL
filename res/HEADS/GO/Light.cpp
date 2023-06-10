#include "Light.h"

Light::Light(LightType type, float power, glm::vec3 color)
{
	o_type = GO_LIGHT;
	light_type = type;
	light_power = power;
	light_color = color;

	const auto [_type, _name] = ParseLightName(light_type);

	light_spirit.spr_type = _type;
	light_spirit.SetTex();

	o_name = _name + std::to_string(GetObjectID());
}

inline  std::pair<SpiritType, std::string> Light::ParseLightName(LightType _type)
{
	switch (_type)
	{
	case NONELIGHT:
		return { POINT_LIGHT_SPIRIT, "None Light"   };
	case POINTLIGHT:
		return { POINT_LIGHT_SPIRIT, "Point Light." };
	case SUNLIGHT:
		return { SUN_LIGHT_SPIRIT,   "Sun."			};
	case SPOTLIGHT:
		return { SPOT_LIGHT_SPIRIT,  "Spot Light."  };
	default:
		return { POINT_LIGHT_SPIRIT, "None Light"   };
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
	light_power = std::abs(_power);
}

void Light::SetShadow(bool _state)
{
	if (_state == use_shadow) return;

	is_light_changed = true;
	use_shadow = _state;
}

void Light::SetRadius(float _rad)
{
	if (_rad == light_radius) return;

	is_light_changed = true;
	light_radius = _rad;
}

void Light::SetAngle(float _ang)
{
	if (_ang == spot_angle) return;

	is_light_changed = true;
	spot_angle = _ang;
}

void Light::RenderLightSpr(Camera* cam)
{
	light_spirit.RenderSpirit(vec3_stdVec6(o_position, light_color), cam);
}

LightArrayBuffer::~LightArrayBuffer()
{
	point_buffer.DeleteBuffer();
	sun_buffer.DeleteBuffer();
	spot_buffer.DeleteBuffer();
	info.DeleteBuffer();
}

void LightArrayBuffer::Init()
{
	point_buffer = StorageBuffer(CUSTOM_LIST, 0);
	sun_buffer   = StorageBuffer(CUSTOM_LIST, 1);
	spot_buffer  = StorageBuffer(CUSTOM_LIST, 2);
	info = UniformBuffer<SceneInfo>(3);
}

void LightArrayBuffer::Bind() const
{
	point_buffer.BindBufferBase();
	sun_buffer.BindBufferBase();
	spot_buffer.BindBufferBase();
	info.Bind(0);
}

void LightArrayBuffer::ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list)
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
				light.second->light_color,
				light.second->o_position,

				light.second->light_power,
				(int)light.second->use_shadow,
				light.second->light_radius,
				});
			break;
		case SUNLIGHT:
			sun.emplace_back(SunStruct{
				light.second->light_color,
				light.second->o_position,
				glm::cross(light.second->o_dir_up, light.second->o_dir_right),

				light.second->light_power,
				(int)light.second->use_shadow,
				});
			break;
		case SPOTLIGHT:
			spot.emplace_back(SpotStruct{
				light.second->light_color,
				light.second->o_position,
				glm::cross(light.second->o_dir_up, light.second->o_dir_right),

				light.second->light_power,
				(int)light.second->use_shadow,
				light.second->spot_angle,
				.1f,
				light.second->light_radius
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

LightArrayBuffer::SceneInfo LightArrayBuffer::GetSceneInfo() const
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

GLsizei LightArrayBuffer::GetTotalCount() const
{
	return point.size() + sun.size() + spot.size();
}
