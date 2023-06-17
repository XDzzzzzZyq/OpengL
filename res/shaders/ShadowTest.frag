#version 430 core

layout(location = 0) out vec4 Output;

struct PointLight{
	vec3 color;
	vec3 pos;

	float power;
	int use_shadow;
	float radius;
};

struct SunLight{
	vec3 color;
	vec3 pos;
	vec3 dir;

	float power;
	int use_shadow;
	mat4 proj_trans;
};

struct SpotLight{
	vec3 color;
	vec3 pos;
	vec3 dir;

	float power;
	int use_shadow;
	float cutoff;
	float outer_cutoff;
};

struct AreaLight{
	vec3 color;

	float power;
	int use_shadow;
	int n;
};

struct AreaVert{
	vec3 v;
};

layout(std430, binding = 0) buffer point_array {
	PointLight point_lights[];
};
layout(std430, binding = 1) buffer sun_array {
	SunLight   sun_lights[];
};
layout(std430, binding = 2) buffer spot_array {
	SpotLight  spot_lights[];
};
layout(std430, binding = 3) buffer area_array {
    AreaLight  area_lights[];
};
layout(std430, binding = 4) buffer area_verts_array {
    AreaVert   area_verts[];
};
layout(std140) uniform SceneInfo {
	int point_count;
	int sun_count;
	int spot_count;
	int area_count;
} scene_info;

// passes
uniform sampler2D U_pos;
uniform sampler2D U_alpha;

uniform sampler2D shadow_test;

in vec2 screen_uv;

vec3 GetCoord(vec3 pos, mat4 trans){

	vec4 pos_p = trans * vec4(pos, 1);
	pos_p.xyz = pos_p.xyz/pos_p.w;
	vec3 coord = vec3(pos_p) * 0.5 + 0.5;
	
	return coord;
}

float evaluateShadow(float c_depth, float min_depth){
	float bias = 0.02;
	float range = 0.005;
	float delta = c_depth - min_depth - bias;  // delta greater/equal than 0

	return 1-smoothstep(0, range, delta);
}

void main(){
	float Alpha = texture2D(U_alpha, screen_uv).r;
	if(Alpha < 0.5){
		Output = vec4(vec3(0.2)*(1-Alpha), 1);
		return;
	}

	/* [Block : DATA] */
	float near = -20;
	float far = 20;

	vec4 Pos_Dep = texture2D(U_pos, screen_uv).rgba;
	vec3 Pos = Pos_Dep.xyz;

	SunLight sun = sun_lights[0];
	
	vec3 coord = GetCoord(Pos, sun.proj_trans);

	float closestDepth = texture(shadow_test, coord.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = coord.z;
    // check whether current frag pos is in shadow
    float shadow = evaluateShadow(currentDepth, closestDepth);



	Output = vec4(vec3(shadow), 1);
	//Output = texture(shadow_test, screen_uv);
	//Output = vec4(vec3(AO), 1);
}
