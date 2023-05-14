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

// input
uniform vec3 Cam_pos;
uniform sampler2D Envir_Texture;
uniform sampler2D Envir_Texture_diff;

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

vec3 Vec3Bisector(vec3 a, vec3 b) {
	return normalize(normalize(a) * 0.5 + normalize(b) * 0.5);
}

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 genHdrUV(vec3 dir) {
	vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
	uv *= invAtan;
	uv += 0.5;
	return -uv + vec2(0.5, 1);
} 

float map(float x, float ai, float bi, float ao, float bo){
	x = (x - ai)/(bi-ai);
	x = clamp(x, 0, 1);
	return ao+(bo-ao)*x;
}

void main(){

	vec3 Pos = texture2D(U_pos, screen_uv).rgb;
	vec3 Normal = texture2D(U_normal, screen_uv).rgb;
	vec3 Albedo = texture2D(U_albedo, screen_uv).rgb;

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

	float F0 = mix(0.1, 1.0, Metalness);
	float Fresnel = mix(exp2((-5.55437*Cos-6.98314)*Cos), 1.0, F0);
	float ks = Fresnel;

	//vec4 uvcolor = texture(U_Texture, uv);
	vec3 reflect_spec = textureLod(Envir_Texture_diff, genHdrUV(-ReflectRay), Roughness * 5).rgb;
	vec3 reflect_diff = textureLod(Envir_Texture_diff, genHdrUV(-Normal), 5).rgb;
	//color = uvcolor * vec4(LightMap.Diffuse_map + LightMap.Specular_map*2, 1.0f);
	//float coef = blen/5;
	Output = vec4(0);

	/* [Block : PBR] */ 
	if(Alpha < 0.05){
		Output += vec4(Emission * Emission_Color, 1);
	}else{
		Output += vec4(mix(reflect_diff, reflect_spec, Fresnel), 1.0f);
		Output += vec4(Emission * Emission_Color, 1);
	}
	// Select = map(Select, 0.5, 0.55, 1, 0)* map(Select, 0.2, 0.25, 0, 1);
	// Output += vec4(vec3(Select), 1);
	Output.a = 1;
	//Output = vec4(normalize(CamRay), 1);
	//Output = vec4(Normal.r, 0, 0, 1);
	//Output = MRSE;
	//Output = vec4(vec3(Alpha), 1);

	//Output = vec4(texture(U_color, screen_uv).rgb, 1);
}
