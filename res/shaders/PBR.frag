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
uniform sampler2D U_emission;
uniform sampler2D U_pos;
uniform sampler2D U_normal;
uniform sampler2D U_albedo;
uniform sampler2D U_mrse;
uniform sampler2D U_color;
uniform sampler2D U_alpha;

uniform sampler2D Envir_Texture_diff;
uniform sampler2D Envir_Texture_spec;
uniform sampler2D LUT;

// input
uniform vec3 Cam_pos;

uniform float gamma;

in vec2 screen_uv;

const float PI = 3.1415926;

// LTC lookup tables
uniform sampler2D LTC1;
uniform sampler2D LTC2;

const float LUT_SIZE = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0) / LUT_SIZE;
const float LUT_BIAS = 0.5 / LUT_SIZE;

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

vec3 Vec3Bisector(vec3 a, vec3 b) {
	return normalize(normalize(a) + normalize(b));
}

vec2 genHdrUV(vec3 dir) {
	vec2 uv = vec2(atan(dir.y, dir.x), asin(dir.z));
	uv *= -vec2(0.15915494, 0.31830988);
	uv += 0.5;
	return uv;
}

float map(float x, float ai, float bi, float ao, float bo){
	x = (x - ai)/(bi-ai);
	x = clamp(x, 0, 1);
	return ao+(bo-ao)*x;
}

float k_light(float r){
	return (r+1)*(r+1)/8;
}

float k_IBL(float r){
	return r*r/2;
}

float fresnel(float Cos, float F0){
	return mix(exp2((-5.55437*Cos-6.98314)*Cos), 1.0, F0);
}

vec3 fresnelSchlick(float Cos, vec3 F0){
	return vec3(fresnel(Cos,F0.x), fresnel(Cos,F0.y), fresnel(Cos,F0.z));
}

float DistributionGGX(float NdotH, float a)
{
    float a2     = a*a;
    float NdotH2 = NdotH*NdotH;

    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float k)
{
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

vec3 BRDF(float NdotL, float NdotV, vec3 V, vec3 N, vec3 L, float Roughness, float Metalness, float Specular, vec3 Albedo, vec3 F0){
	vec3 H = Vec3Bisector(L, V);
	float HdotV = max(dot(V, L), 0);
	float NdotH = max(dot(N, H), 0);

	float NDF = DistributionGGX(NdotH, Roughness);
	float G = GeometrySmith(NdotV, NdotL, k_light(Roughness));

	vec3 Fd = fresnelSchlick(NdotV, F0);

	vec3 kSd = Fd;
	vec3 kDd = vec3(1.0) - kSd;
	kDd *= 1.0 - Metalness;

	vec3 numerator    = NDF * G * Fd;
	float denominator = 4.0 * NdotV * NdotL + 0.0001;
	vec3 specular     = numerator / denominator;

	return kDd * Albedo / PI + numerator/denominator;
}

// Compute acos(dot(v1, v2)) * normalize(cross(v1, v2))
// https://learnopengl.com/Guest-Articles/2022/Area-Lights
vec3 IntegrateEdge(vec3 v1, vec3 v2)
{
	float x = dot(v1, v2);
	float y = abs(x);

	float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, int i0, int n)
{
	// Construct orthonormal basis around N
	vec3 T1, T2;
	T1 = normalize(V - N * dot(V, N));
	T2 = cross(N, T1);
	Minv = Minv * transpose(mat3(T1, T2, N));

	// Integrate vector irradiance over the edges
	vec3 vsum = vec3(0.0f);
	for (int i = 0; i < n; ++i)
	{
		// Transform light direction from LTC to cosine weighted space
		vec3 L0 = Minv * (area_verts[i0 + i].v - P);
		vec3 L1 = Minv * (area_verts[i0 + (i + 1) % n].v - P);
		L0 = normalize(L0);
		L1 = normalize(L1);

		vsum += IntegrateEdge(L0, L1);
	}

	// Check if the point is behind the light
	vec3 dir = P - area_verts[i0].v;
	vec3 lightNormal = cross(area_verts[i0 + 1].v - area_verts[i0].v, area_verts[i0 + 2].v - area_verts[i0].v);
	bool behind = (dot(dir, lightNormal) < 0.0);

	// Form factor
	float len = length(vsum);
	// Projection on tangent surface
	float z = vsum.z / len;
	if (behind) z = -z;

	vec2 uv = vec2(z * 0.5f + 0.5f, len);
	uv = uv * LUT_SCALE + LUT_BIAS;

	// Fetch the form factor for horizon clipping
	float scale = texture(LTC2, uv).w;
	float sum = len * scale;
	if (behind) sum = 0.0f;

	return vec3(sum);
}

void main(){

	/* [Block : DATA] */

	vec4 Pos_Dep = texture2D(U_pos, screen_uv).rgba;
	vec3 Pos = Pos_Dep.xyz;
	vec4 Normal_AO = texture2D(U_normal, screen_uv).rgba;
	vec3 Normal = normalize(Normal_AO.xyz);
	vec3 Albedo = texture2D(U_albedo, screen_uv).rgb;

	float AO = Normal_AO.a;

	//Albedo *= AO;

	vec3 CamRay = normalize(Pos - Cam_pos);
	vec3 ReflectRay = reflect(CamRay, Normal);
	float NdotV = max(dot(Normal, -CamRay), 0);

	vec4 MRSE = texture2D(U_mrse, screen_uv).rgba;
	float Metalness = clamp(MRSE.r, 0.001, 0.999);
	float Roughness = clamp(MRSE.g, 0.001, 0.999);
	float Specular = MRSE.b;
	float Emission = MRSE.a;

	vec3 Emission_Color = texture2D(U_emission, screen_uv).rgb;

	float Alpha = texture2D(U_alpha, screen_uv).r;
	float Select = texture2D(U_alpha, screen_uv).g;

	vec3 F0 = mix(vec3(0.1), Albedo, Metalness);
	vec3 Fresnel = fresnelSchlick(NdotV, F0);
	vec3 ks = Fresnel;

	//vec4 uvcolor = texture(U_Texture, uv);
	//color = uvcolor * vec4(LightMap.Diffuse_map + LightMap.Specular_map*2, 1.0f);
	//float coef = blen/5;
	Output = vec4(0);

	/* [Block : BG] */

	if(Alpha < 0.05){
		Output += vec4(Emission * Emission_Color, 1);
		Output = Vec4Film(Output, 1, gamma);
		//Output = texture2D(LUT, screen_uv);
		return;
	}


	/* [Block : Lighting] */

	vec3 Light_res = vec3(0);

	for(uint i = 0; i<scene_info.point_count; i++){
		PointLight light = point_lights[i];
		vec3 toLight = Pos - light.pos;
		float dist = length(toLight);
		vec3 L = normalize(toLight);

		float Attenuation = 1.0 / (dist * dist);
		float NdotL = max(dot(Normal, L), 0);
		vec3 Radiance = light.power * light.color * Attenuation * NdotL;
		Light_res += BRDF(NdotL, NdotV, -CamRay, Normal, L, Roughness, Metalness, Specular, Albedo, F0) * Radiance;
	}

	for(uint i = 0; i<scene_info.sun_count; i++){
		SunLight light = sun_lights[i];

		vec3 L = normalize(light.dir);
		float NdotL = max(dot(Normal, L), 0);
		vec3 Radiance = light.color * light.power * NdotL;
		Light_res += BRDF(NdotL, NdotV, -CamRay, Normal, L, Roughness, Metalness, Specular, Albedo, F0) * Radiance;
	}

	for(uint i = 0; i < scene_info.spot_count; i++){
		SpotLight light = spot_lights[i];
		vec3 toLight = Pos - light.pos;
		float dist = length(toLight);
		vec3 L = normalize(toLight);

		float theta = dot(-L, normalize(light.dir));
		float epsilon = light.cutoff - light.outer_cutoff;
		float Intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
		float Attenuation = 1.0 / (dist * dist);
		float NdotL = max(dot(Normal, L), 0);
		vec3 Radiance = light.power * light.color * Intensity * Attenuation * NdotL;
		Light_res += BRDF(NdotL, NdotV, -CamRay, Normal, L, Roughness, Metalness, Specular, Albedo, F0) * Radiance;
	}

	// Remove after implementing area lights
	Light_res = vec3(0.0f);

	/* [Block : Area Lights] */

	int i0 = 0;
	for (uint i = 0; i < scene_info.area_count; i++){
	    AreaLight light = area_lights[i];

		vec3 Diffuse = LTC_Evaluate(Normal, -CamRay, Pos, mat3(1), i0, light.n);
		Light_res += light.power * light.color * Diffuse;

		i0 += light.n;
	}

	/* [Block : IBL] */

	vec3 IBL_res = vec3(0);
	vec2 lut = texture2D(LUT, vec2(clamp(NdotV, 0.0, 0.99), Roughness)).xy;

	//ReflectRay = normalize(mix(ReflectRay, Normal, Roughness));
	vec3 reflect_spec = textureLod(Envir_Texture_spec, genHdrUV(-ReflectRay), Roughness * 7).rgb;
	reflect_spec *= Fresnel*lut.x + lut.y;
	vec3 reflect_diff = textureLod(Envir_Texture_diff, genHdrUV(-Normal), 4).rgb * Albedo;

	IBL_res += reflect_diff * (1-Metalness) + reflect_spec * Specular;

	/* [Block : EMIS] */

	IBL_res += Emission * Emission_Color;
	//Output = vec4(lut, 0, 1);


	/* [Block : COMP] */

	Output += vec4(Light_res, 0);
	//Output += vec4(IBL_res, 0);
	Output *= AO;
	Output.a = 1;
	Output = Vec4Film(Output, 1, gamma);

	//Output = texture2D(LUT, screen_uv);
	//Output = vec4(vec3(point_lights[0].power), 1);

	//Output = vec4(texture2D(U_pos, screen_uv).aaa, 1);
}
