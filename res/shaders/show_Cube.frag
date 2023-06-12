#version 430 core

layout(location = 0) out vec4 Output;

uniform sampler2D U_pos;
uniform sampler2D U_normal;

uniform samplerCube Envir_Texture_diff;
uniform samplerCube Envir_Texture_spec;

// input
uniform vec3 Cam_pos;

in vec2 screen_uv;

const float PI = 3.1415926;

vec2 genHdrUV(vec3 dir) {
	vec2 uv = vec2(atan(dir.y, dir.x), asin(dir.z));
	uv *= -vec2(0.15915494, 0.31830988);
	uv += 0.5;
	return uv;
}

void main(){

	/* [Block : DATA] */

	vec4 Pos_Dep = texture2D(U_pos, screen_uv).rgba;
	vec3 Pos = Pos_Dep.xyz;
	vec4 Normal_AO = texture2D(U_normal, screen_uv).rgba;
	vec3 Normal = normalize(Normal_AO.xyz);

	//Albedo *= AO;

	vec3 CamRay = normalize(Pos - Cam_pos);
	vec3 ReflectRay = reflect(CamRay, Normal);

	//vec4 uvcolor = texture(U_Texture, uv);
	//color = uvcolor * vec4(LightMap.Diffuse_map + LightMap.Specular_map*2, 1.0f);
	//float coef = blen/5;
	Output = vec4(0);

	Output = vec4(texture(Envir_Texture_spec, Normal).rgb, 1);
	//Output = vec4(Normal, 1);
}
