#version 430 core

layout(location = 0) out vec4 Output;

// passes
uniform sampler2D U_emission;
uniform sampler2D U_pos;
uniform sampler2D U_normal;
uniform sampler2D U_albedo;
uniform sampler2D U_mrse;
uniform sampler2D U_color;
uniform sampler2D U_alpha;
uniform sampler2D LUT;

// input
uniform vec3 Cam_pos;
uniform sampler2D Envir_Texture_diff;
uniform sampler2D Envir_Texture_spec;

uniform float gamma;

in vec2 screen_uv;

const float PI = 3.1415926;

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

const vec2 invAtan = vec2(0.15915494, 0.31830988);
vec2 genHdrUV(vec3 dir) {
	vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
	uv *= invAtan;
	uv += 0.5;
	uv = -uv + vec2(0.5, 1);
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

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
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
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

void main(){

	/* [Block : DATA] */ 

	vec4 Pos_Dep = texture2D(U_pos, screen_uv).rgba;
	vec3 Pos = Pos_Dep.xyz;
	vec4 Normal_AO = texture2D(U_normal, screen_uv).rgba;
	vec3 Normal = Normal_AO.xyz;
	vec3 Albedo = texture2D(U_albedo, screen_uv).rgb;

	float AO = Normal_AO.a;

	//Albedo *= AO; 

	vec3 CamRay = Pos - Cam_pos;
	vec3 ReflectRay = reflect(normalize(CamRay), Normal);
	float Cos = -dot(Normal, normalize(CamRay));

	vec4 MRSE = texture2D(U_mrse, screen_uv).rgba;
	float Metalness = MRSE.r;
	float Roughness = MRSE.g;
	float Specular = MRSE.b;
	float Emission = MRSE.a;

	vec3 Emission_Color = texture2D(U_emission, screen_uv).rgb;

	float Alpha = texture2D(U_alpha, screen_uv).r;
	float Select = texture2D(U_alpha, screen_uv).g;

	float F0 = mix(0.1, 0.95, Metalness);
	float Fresnel = mix(F0, 1.0, exp2((-5.55437*Cos-6.98314)*Cos));
	float ks = Fresnel;

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

	/* [Block : IBL] */ 

	vec3 IBL_res = vec3(0);
	vec2 lut = texture2D(LUT, vec2(Cos, Roughness)).xy;

	//ReflectRay = normalize(mix(ReflectRay, Normal, Roughness));
	vec3 reflect_spec = textureLod(Envir_Texture_spec, genHdrUV(-ReflectRay), Roughness * 7).rgb;
	reflect_spec *= Fresnel*lut.x + lut.y;
	vec3 reflect_diff = textureLod(Envir_Texture_diff, genHdrUV(-Normal), 4).rgb * Albedo;
	
	IBL_res += reflect_diff + reflect_spec;

	/* [Block : EMIS] */

	IBL_res += Emission * Emission_Color;
	//Output = vec4(lut, 0, 1);


	/* [Block : COMP] */ 

	Output += vec4(Light_res, 0);
	Output += vec4(IBL_res, 0);
	Output *= AO;
	Output.a = 1;
	Output = Vec4Film(Output, 1, gamma);

	//Output = texture2D(LUT, screen_uv);
	//Output = vec4(vec3(Normal_AO.a), 1);

	//Output = vec4(texture2D(U_pos, screen_uv).aaa, 1);
}
