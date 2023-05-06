#include "Light.h"

Light::Light(LightType type, float power, glm::vec3 color)
{
	o_type = GO_LIGHT;
	light_type = type;
	light_power = power;
	light_color = color;
	GenFloatData();

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

	}
}

void Light::RenderLightSpr(Camera* cam)
{
	light_spirit.RenderSpirit(vec3_stdVec6(o_position, light_color), cam);
}

void Light::GenFloatData() const
{
	light_floatData = {
		(float)is_shadow,
		light_power,

		light_color[0],
		light_color[1],
		light_color[2],

		o_position[0],
		o_position[1],
		o_position[2],

		o_rot[0],
		o_rot[1],
		o_rot[2],

		spot_angle,
		spot_radius,
	};
	//std::cout << o_position[0] << "\n";
}
