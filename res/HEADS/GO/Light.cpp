#include "Light.h"

Light::Light(LightType type, float power, glm::vec3 color)
{
	light_type = type;
	light_power = power;
	light_color = color;
	GenFloatData();

	light_spirit.spr_type = LIGHTSPIRIT;
	light_spirit.SetTex();
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
