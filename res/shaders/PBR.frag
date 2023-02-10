layout(location = 0) out vec4 color;

uniform sampler2D U_combine;
uniform sampler2D U_pos;
uniform sampler2D U_normal;
uniform sampler2D U_albedo;
uniform sampler2D U_emit;
uniform sampler2D U_metal;
uniform sampler2D U_rough;
uniform sampler2D U_spec;
uniform sampler2D U_emit_str;
uniform sampler2D U_color;

in vec2 screen_uv;

float ACESFilm(float x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

vec3 Vec3Film(vec3 x, float fac, float gamma)
{
	x[0] = ACESFilm(x[0] * fac);
	x[1] = ACESFilm(x[1] * fac);
	x[2] = ACESFilm(x[2] * fac);
	return pow(x, vec3(1.0/gamma));
}

vec4 Vec4Film(vec4 x, float fac, float gamma)
{
	x.rgb = Vec3Film(x.rgb, fac, gamma);
	return x;
}

void main(){
	color = texture(U_color, screen_uv);
}