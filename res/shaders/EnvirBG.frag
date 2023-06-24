#version 430 core

layout(location = 0) out vec4 EMIScolor;
layout(location = 1) out vec4 POScolor;
layout(location = 2) out vec4 NORMALcolor;
layout(location = 4) out vec4 MRSEcolor;
layout(location = 5) out vec4 RANDcolor;
layout(location = 6) out vec4 IDcolor;
layout(location = 7) out vec4 MASKcolor;

in vec2 screen_uv;
uniform samplerCube hdr_texture;

uniform mat4 cam_rotM;
uniform float cam_fov;
uniform float cam_ratio;

uniform vec3 ID_color;
uniform vec3 RAND_color;

vec4 ray_dir;
vec2 reUV;
vec2 equirectangularUV;

vec2 genHdrUV(vec3 dir) {
	vec2 uv = vec2(atan(dir.y, dir.x), asin(dir.z));
	uv *= -vec2(0.15915494, 0.31830988);
	uv += 0.5;
	return uv;
}

vec2 reMapUV(vec2 uv) {
	return uv * 2 - 1;
}

vec4 GetSelect(vec4 col, float act) {
	col[3] = 1.0f;
	col -= vec4((act + 1) / 256, 0, 0, 0);
	if (col[0] >= 0 || col[0] <= -0.005) {
		col = vec4(1, 1, 1, 1);
	}
	return col;
}

void main() {
	
	IDcolor = vec4(ID_color, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	MASKcolor = vec4(0, 0, 0, 1);

	reUV =  reMapUV(screen_uv);
	ray_dir = cam_rotM * vec4(vec3(1, 1, -1) * normalize(vec3(reUV * tan(cam_fov / 2) * vec2(cam_ratio, 1), 1)), 1);

	NORMALcolor = ray_dir;
	POScolor = vec4(vec3(-1000), 1);
	
	EMIScolor = vec4(textureLod(hdr_texture,normalize(vec3(ray_dir)), 0).rgb, 1.0f);
	MRSEcolor = vec4(0, 0, 0, 1);
};