#version 430 core

layout(location = 0) out vec4 Output;
layout(location = 1) out vec4 OutDirDiff;
layout(location = 2) out vec4 OutDirSpec;
layout(location = 3) out vec4 OutIndDiff;
layout(location = 4) out vec4 OutIndSpec;
layout(location = 5) out vec4 OutEmission;

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
	mat4 trans;

	float power;
	int use_shadow;
	float ratio;
};

struct PolygonLight{
	vec3 color;

	float power;
	int use_shadow;
	int n;
};

struct PolyLightVert{
	vec3 v;
};

struct Diff_Spec{
	vec3 diff;
	vec3 spec;
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
layout(std430, binding = 4) buffer polygon_array {
    PolygonLight  polygon_lights[];
};
layout(std430, binding = 5) buffer polygon_verts_array {
    PolyLightVert   polygon_verts[];
};
layout(std140) uniform SceneInfo {
	int point_count;
	int sun_count;
	int spot_count;
	int area_count;
	int polygon_count;
} scene_info;

// passes
uniform sampler2D U_emission;
uniform sampler2D U_pos;
uniform sampler2D U_normal;
uniform sampler2D U_albedo;
uniform sampler2D U_mrse;
uniform sampler2D U_color;
uniform sampler2D U_alpha;

uniform samplerCube Envir_Texture_diff;
uniform samplerCube Envir_Texture_spec;
uniform sampler2D U_Shadow[16];

uniform float point_far;

// input
uniform vec3 Cam_pos;
in vec2 screen_uv;

const float PI = 3.1415926;

// LTC lookup tables
uniform sampler2D LTC1;
uniform sampler2D LTC2;

const float LUT_SIZE = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0) / LUT_SIZE;
const float LUT_BIAS = 0.5 / LUT_SIZE;

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

vec3 fresnelSG(float Cos, vec3 F0){
	return mix(vec3(exp2((-5.55437*Cos-6.98314)*Cos)), vec3(1.0), F0);
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

Diff_Spec BRDF(float NdotL, float NdotV, vec3 V, vec3 N, vec3 L, float Roughness, float Metalness, vec3 Albedo, vec3 F0){
	vec3 H = Vec3Bisector(L, V);
	float HdotV = max(dot(V, L), 0);
	float NdotH = max(dot(N, H), 0);

	float NDF = DistributionGGX(NdotH, Roughness);
	float G = GeometrySmith(NdotV, NdotL, k_light(Roughness));

	vec3 Fd = fresnelSG(NdotV, F0);

	vec3 kSd = Fd;
	vec3 kDd = vec3(1.0) - kSd;
	kDd *= 1.0 - Metalness;

	vec3 numerator    = NDF * G * Fd;
	float denominator = 4.0 * NdotV * NdotL + 0.0001;
	vec3 specular     = numerator / denominator;

	Diff_Spec result;
	result.diff = kDd * Albedo / PI;
	result.spec = numerator/denominator;

	return result;
}

// Compute acos(dot(v1, v2)) * normalize(cross(v1, v2))
// https://learnopengl.com/Guest-Articles/2022/Area-Lights
vec3 IntegrateEdge(vec3 v1, vec3 v2){

	float x = dot(v1, v2);
	float y = abs(x);

	float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

vec4 Vector_Irradiance_Area(vec3 N, vec3 V, vec3 P, mat3 Minv, mat4 Area_T, float ratio){
	vec3 T1, T2;
	T1 = normalize(V - N * dot(V, N));
	T2 = cross(N, T1);
	Minv = Minv * transpose(mat3(T1, T2, N));


	vec3 L0 = vec3(Area_T * vec4( ratio, 1, 0, 1)) - P;
	vec3 L1 = vec3(Area_T * vec4(-ratio, 1, 0, 1)) - P;
	vec3 L2 = vec3(Area_T * vec4(-ratio,-1, 0, 1)) - P;
	vec3 L3 = vec3(Area_T * vec4( ratio,-1, 0, 1)) - P;

	vec3 dir = normalize(P - vec3(Area_T * vec4(vec3(0), 1)));
	vec3 lightNormal = normalize(mat3(Area_T)*vec3(0, 0, 1));
	bool behind = (dot(dir, lightNormal) < 0.0);

		// Transform light direction from LTC to cosine weighted space
	L0 = normalize(Minv * L0);
	L1 = normalize(Minv * L1);
	L2 = normalize(Minv * L2);
	L3 = normalize(Minv * L3);
	vec3 vsum = vec3(0.0f);
	vsum += IntegrateEdge(L0, L1);
	vsum += IntegrateEdge(L1, L2);
	vsum += IntegrateEdge(L2, L3);
	vsum += IntegrateEdge(L3, L0);

	return vec4(vsum, behind);
}

vec4 Vector_Irradiance_Poly(vec3 N, vec3 V, vec3 P, mat3 Minv, int i0, int n){

	// Construct orthonormal basis around N
	vec3 T1, T2;
	T1 = normalize(V - N * dot(V, N));
	T2 = cross(N, T1);
	Minv = Minv * transpose(mat3(T1, T2, N));

	vec3 vsum = vec3(0.0f);
	for (int i = 0; i < n; ++i)
	{
		// Transform light direction from LTC to cosine weighted space
		vec3 L0 = Minv * (polygon_verts[i0 + i].v - P);
		vec3 L1 = Minv * (polygon_verts[i0 + (i + 1) % n].v - P);

		vsum += IntegrateEdge(normalize(L0), normalize(L1));
	}

	vec3 dir = P - polygon_verts[i0].v;
	vec3 lightNormal = cross(polygon_verts[i0 + 1].v - polygon_verts[i0].v, polygon_verts[i0 + 2].v - polygon_verts[i0].v);
	bool behind = (dot(dir, lightNormal) < 0.0);

	return vec4(vsum, behind);
}

vec3 LTC_Evaluate(vec4 Vec_Irrad)
{
	// Integrate vector irradiance over the edges
	vec4 vec_irrad = Vec_Irrad;
	vec3 vsum = vec_irrad.xyz;

	// Check if the point is behind the light
	bool behind = bool(vec_irrad.w);
	if(behind)
		return vec3(0);

	// Form factor
	float len = length(vsum);
	// Projection on tangent surface
	float z = vsum.z / len;
	// The value of z appears to be reversed during LUT lookup
	if (!behind) z = -z;

	vec2 uv = vec2(z * 0.5f + 0.5f, len);
	uv = uv * LUT_SCALE + LUT_BIAS;

	// Fetch the form factor for horizon clipping
	float scale = texture(LTC2, uv).w;
	float sum = len * scale;

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
	vec3 Fresnel = fresnelSG(NdotV, F0);
	vec3 ks = Fresnel;

	float dotNV = clamp(dot(Normal, normalize(-CamRay)), 0.0f, 1.0f);
	vec2 uv = vec2(Roughness, sqrt(1.0f - dotNV));
	uv = uv * LUT_SCALE + LUT_BIAS;

	// Get 4 parameters for inverse M
	vec4 t1 = texture(LTC1, uv);
	// Get 2 parameters for Fresnel calculation
	vec4 t2 = texture(LTC2, uv);

	mat3 Minv = mat3(
		vec3(t1.x, 0, t1.y),
		vec3(   0, 1,    0),
		vec3(t1.z, 0, t1.w)
	);

	//vec4 uvcolor = texture(U_Texture, uv);
	//color = uvcolor * vec4(LightMap.Diffuse_map + LightMap.Specular_map*2, 1.0f);
	//float coef = blen/5;
	Output = vec4(0);

	/* [Block : BG] */

	if(Alpha < 0.05){
		Output += vec4(Emission * Emission_Color, 1);
		//Output = texture2D(LUT, screen_uv);
		return;
	}

	vec3 Dir_Diff = vec3(0);
	vec3 Dir_Spec = vec3(0);
	vec3 Ind_Diff = vec3(0);
	vec3 Ind_Spec = vec3(0);

	/* [Block : Lighting] */

	for(uint i = 0; i<scene_info.point_count; i++){
		PointLight light = point_lights[i];
		vec3 toLight = light.pos - Pos;
		float dist = length(toLight);
		vec3 L = normalize(toLight);

		float shadow = texture(U_Shadow[i], screen_uv).r;
		if (shadow < 0.01) continue;

		float Attenuation = 1.0 / (dist * dist);
		float NdotL = max(dot(Normal, L), 0);
		vec3 Radiance = light.power * light.color * Attenuation * NdotL * shadow;
		Diff_Spec result = BRDF(NdotL, NdotV, -CamRay, Normal, L, Roughness, Metalness, Albedo, F0);

		Dir_Diff += result.diff * Radiance;
		Dir_Spec += result.spec * Radiance;
	}

	for(uint i = 0; i<scene_info.sun_count; i++){
		SunLight light = sun_lights[i];

		float shadow = texture(U_Shadow[scene_info.point_count+i], screen_uv).r;
		if (shadow < 0.01) continue;

		vec3 L = -light.dir;
		float NdotL = max(dot(Normal, L), 0);
		vec3 Radiance = light.color * light.power * NdotL * shadow;

		Diff_Spec result = BRDF(NdotL, NdotV, -CamRay, Normal, L, Roughness, Metalness, Albedo, F0);

		Dir_Diff += result.diff * Radiance;
		Dir_Spec += result.spec * Radiance;
	}

	for(uint i = 0; i < scene_info.spot_count; i++){
		SpotLight light = spot_lights[i];
		vec3 toLight = light.pos - Pos;
		float dist = length(toLight);
		vec3 L = normalize(toLight);

		float shadow = texture(U_Shadow[scene_info.point_count+scene_info.sun_count+i], screen_uv).r;
		if (shadow < 0.01) continue;

		float theta = dot(-L, light.dir);
		float epsilon = light.cutoff - light.outer_cutoff;
		float Intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
		float Attenuation = 1.0 / (dist * dist);
		float NdotL = max(dot(Normal, L), 0);
		vec3 Radiance = light.power * light.color * Intensity * Attenuation * NdotL * shadow;

		Diff_Spec result = BRDF(NdotL, NdotV, -CamRay, Normal, L, Roughness, Metalness, Albedo, F0);

		Dir_Diff += result.diff * Radiance;
		Dir_Spec += result.spec * Radiance;
	}

	for (uint i = 0; i < scene_info.area_count; i++){
	    AreaLight light = area_lights[i];

		float shadow = texture(U_Shadow[scene_info.point_count+scene_info.sun_count+scene_info.spot_count+i], screen_uv).r;
		if (shadow < 0.01) continue;

		vec4 vec_irrad_spec = Vector_Irradiance_Area(Normal, -CamRay, Pos, mat3(1), light.trans, light.ratio);
		vec3 diffuse = LTC_Evaluate(vec_irrad_spec);
		vec4 vec_irrad_diff = Vector_Irradiance_Area(Normal, -CamRay, Pos, Minv   , light.trans, light.ratio);
		vec3 specular = LTC_Evaluate(vec_irrad_diff);

		// GGX BRDF shadowing and Fresnel
		// NOTE: I am not certain about mixing these two terms with the Specular factor
		//       This is simply following the implementation on learnopengl.com
		specular *= mix(t2.x, t2.y, Specular);

		Dir_Diff += light.power * light.color * (Albedo * diffuse) * shadow;
		Dir_Spec += light.power * light.color * specular * shadow;
	}

	/* [Block : Polygon Lights] */

	int i0 = 0;
	for (uint i = 0; i < scene_info.polygon_count; i++){
	    PolygonLight light = polygon_lights[i];

		vec4 vec_irrad_spec = Vector_Irradiance_Poly(Normal, -CamRay, Pos, mat3(1), i0, light.n);
		vec3 diffuse = LTC_Evaluate(vec_irrad_spec);
		vec4 vec_irrad_diff = Vector_Irradiance_Poly(Normal, -CamRay, Pos, Minv   , i0, light.n);
		vec3 specular = LTC_Evaluate(vec_irrad_diff);

		// GGX BRDF shadowing and Fresnel
		// NOTE: I am not certain about mixing these two terms with the Specular factor
		//       This is simply following the implementation on learnopengl.com
		specular *= mix(t2.x, t2.y, Specular);

		Dir_Diff += light.power * light.color * (Albedo * diffuse);
		Dir_Spec += light.power * light.color * specular;

		i0 += light.n;
	}

	/* [Block : IBL] */

	//ReflectRay = normalize(mix(ReflectRay, Normal, Roughness));
	vec3 reflect_spec = textureLod(Envir_Texture_spec, ReflectRay, sqrt(Roughness) * 7).rgb;
	reflect_spec *= Fresnel*t2.x + t2.y;
	vec3 reflect_diff = texture(Envir_Texture_diff, Normal).rgb * Albedo;

	Ind_Diff += reflect_diff * (1-Metalness);
	Ind_Spec += reflect_spec * Specular;

	/* [Block : EMIS] */

	Output += vec4(Emission * Emission_Color, 0);
	//Output = vec4(lut, 0, 1);


	/* [Block : COMP] */

	vec3 Light_res = (Dir_Diff + Ind_Diff) + (Dir_Spec + Ind_Spec);

	Output += vec4(Light_res, 0);
	Output.a = 1;

	OutDirDiff = vec4(Dir_Diff, 1);
	OutDirSpec = vec4(Dir_Spec, 1);
	OutIndDiff = vec4(Ind_Diff, 1);
	OutIndSpec = vec4(Ind_Spec, 1);
	OutEmission = vec4(Emission * Emission_Color, Emission);
}
