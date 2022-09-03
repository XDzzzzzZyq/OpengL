#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aScreen_uv;

out vec2 screen_uv;

void main() {

	screen_uv = aScreen_uv;
	gl_Position = vec4(aPos, 0.0f, 1.0f);

};








#shader fragment
#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;

in vec2 screen_uv;
vec4 screen_color;
vec4 hdr_color;
vec4 outline;

uniform sampler2D screen_texture;
uniform sampler2D id_texture;
uniform sampler2D hdr_texture;
uniform sampler2D select_texture;
uniform vec3 pure_color;

uniform mat4 cam_rotM;
uniform float cam_fov;
uniform float cam_ratio;
uniform vec3 ID_color;
uniform vec3 RAND_color;

uniform float activeID;
uniform float U_gamma;

vec4 ray_dir;
vec2 reUV;
vec2 equirectangularUV;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 genHdrUV(vec3 dir) {
	vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
	uv *= invAtan;
	uv += 0.5;
	return -uv + vec2(0.5, 1);
}

vec2 reMapUV(vec2 uv) {
	return uv * 2 - vec2(1.0f, 1.0f);
}

vec4 GetSelect(vec4 col, float act) {
	col[3] = 1.0f;
	col -= vec4((act + 1) / 256, 0, 0, 0);
	if (col[0] >= 0 || col[0] <= -0.005) {
		col = vec4(1, 1, 1, 1);
	}
	return col;
}

vec2 tex_offset = 1.0 / textureSize(select_texture, 0);

vec4 Conv3_3(sampler2D tex, vec2 uv, int rad) {
	vec4 result = vec4(0.0f);
	vec2 offest = 1.5 * vec2(1) * tex_offset;
	for (int i = -rad; i <= rad; i++) {
		for (int j = -rad; j <= rad; j++) {
			result += texture(tex, uv + offest * vec2(i, j)) / pow(2 * rad + 1, 2);
		}
	}
	return vec4(result[3]);
}

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

void main() {

	reUV =  reMapUV(screen_uv);
	ray_dir = cam_rotM * vec4(vec3(-1, -1, 1) * normalize(vec3(reUV * tan(cam_fov / 2) * vec2(cam_ratio, 1), 1)), 1);

	screen_color = Vec4Film(texture(screen_texture, screen_uv), 1.0f, U_gamma);
	hdr_color = vec4(Vec3Film(texture(hdr_texture, genHdrUV(normalize(vec3(ray_dir)))).rgb, 1.0f, U_gamma), 1.0f);
	//hdr_color = vec4(texture((hdr_texture,SampleSphericalMap(normalize(vec3(ray_dir)))1.0f);

	color = hdr_color * (1 - screen_color[3]) + vec4(vec3(screen_color), 1.0f) * screen_color[3];

	if (activeID != 0) {
		IDcolor = texture(select_texture, screen_uv);
		outline = IDcolor * Conv3_3(select_texture, screen_uv, 2);
		outline = vec4(vec3(max(pow(IDcolor[3] * (1 - outline[3]), 0.1), 0)), 1);
		color += outline;
	}

	IDcolor = vec4(ID_color, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};