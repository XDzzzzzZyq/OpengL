#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvIn;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 smo_normal;

out vec2 uv;
out vec4 testcolor;
out vec3 pix_pos;
out vec4 normal_color;
out vec4 Snormal_color;

uniform mat4 U_cam_trans;
uniform mat4 U_obj_trans;
uniform mat4 U_ProjectM;
uniform float z_inp;

void main(){
	uv = uvIn;
	
	gl_Position = U_ProjectM * U_cam_trans * U_obj_trans * vec4(position,1.0f);
	testcolor = vec4(position,1.0f);

	Snormal_color = vec4(normalize(mat3(U_obj_trans) * smo_normal), 1.0f);
	normal_color = vec4(normalize(mat3(U_obj_trans) * normal), 1.0f);
	pix_pos = vec3(U_obj_trans * vec4(position,1.0f));
	//gl_Position = U_ProjectM * position;
};








#shader fragment
#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;
layout(location = 3) out vec4 SELECcolor;

in vec2 uv;
in vec4 testcolor;
in vec3 pix_pos;
in vec4 normal_color;
in vec4 Snormal_color;

uniform sampler2D U_Texture;
uniform sampler2D Envir_Texture;
uniform float blen[3];
uniform vec3 ID_color;
uniform vec3 RAND_color;
uniform int is_selected;

uniform float L_point[41]; //count + 5 points
uniform float L_sun[23]; //count + 2 suns
uniform float L_spot[40]; //count + 3 spots

uniform float Scene_data[8];//   | pos3f | rot3f | ratio1f | angle1f | + 
						    //   |           CAMREA_DATA             | + |       POST_PROCESS        |



struct pointLight{
	float is_shadow;
	vec3 pos;
	vec3 Lcolor;
	float power;
};

struct sunLight{
	float is_shadow;
	vec3 rot;
	vec3 Lcolor;
	float power;
};

struct spotLight{
	float is_shadow;
	vec3 pos;
	vec3 rot;
	vec3 Lcolor;
	float power;
	float angle;
	float radius;
};


pointLight pL_list[5];
sunLight sL_list[2];
spotLight spL_list[3];

struct LightMapStruct{
	vec3 Diffuse_map;
	vec3 Specular_map;
	vec3 Environ_map;
};

struct Camera{
	vec3 cam_pos;
};

float ACESFilm(float x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return (x*(a*x+b))/(x*(c*x+d)+e);
}

vec3 Vec3Film(vec3 x)
{
	x[0] = ACESFilm(x[0]);
	x[1] = ACESFilm(x[1]);
	x[2] = ACESFilm(x[2]);
	return x;
}

vec3 Vec3Bisector(vec3 a, vec3 b){
	return normalize(normalize(a)*0.5 + normalize(b)*0.5);
}

vec2 genHdrUV(vec3 dir){
	float spin = atan(dir[2]/dir[0]);
	float elevation = atan(dir[1]/sqrt(dir[0]*dir[0]+dir[2]*dir[2]));
	if(dir[0]>0){
		spin-=3.1415926f;
	}
	return vec2(((spin/3.1415926f)/2)+0.5, (elevation/-3.1415926f+1)/2);
}

LightMapStruct LightMap;
float Dis_fac, DiffuseStrength, SpecularStrength;

vec3 LightRay, CamRay, ReflectRay;

void main(){
	DiffuseStrength = 0.0f;
	LightMap.Diffuse_map = vec3(0.0f,0.0f,0.0f);
	LightMap.Specular_map = vec3(0.0f,0.0f,0.0f);
	//color = U_color;

	IDcolor=vec4(ID_color/256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);

	//Generate PL_LIST & pL_list Shading
	for(int i = 0;i<L_point[0];i++){
		pL_list[i].is_shadow = pL_list[i].power = L_point[1+i*8];
		pL_list[i].power = L_point[2+i*8];

		pL_list[i].Lcolor = vec3(L_point[3+i*8], L_point[4+i*8], L_point[5+i*8]);

		pL_list[i].pos = vec3(L_point[6+i*8], L_point[7+i*8], L_point[8+i*8]);


		LightRay = pix_pos-pL_list[i].pos;
		CamRay = pix_pos-vec3(Scene_data[0], Scene_data[1], Scene_data[2]);
		ReflectRay = reflect(normalize(CamRay), vec3(Snormal_color));
		Dis_fac = 1/length(LightRay);

		//DIFFUSE
		DiffuseStrength = pL_list[i].power * Dis_fac * Dis_fac * max( dot(-LightRay, vec3(Snormal_color)), 0.0f ) * Dis_fac;
		LightMap.Diffuse_map += pL_list[i].Lcolor * DiffuseStrength;

		//SPECULAR
		SpecularStrength = pL_list[i].power * Dis_fac * Dis_fac *pow( max( -dot(Vec3Bisector(CamRay,LightRay), vec3(Snormal_color)), 0.0f), 128.0f);
		LightMap.Specular_map += pL_list[i].Lcolor * SpecularStrength;
	}					   
						   
	vec4 uvcolor = texture(U_Texture,uv);
	vec3 reflectcolor = vec3(texture(Envir_Texture, genHdrUV(-ReflectRay)));
	SELECcolor = vec4(is_selected);

	//color = uvcolor * vec4(Vec3Film(LightMap.Diffuse_map + LightMap.Specular_map*2), 1.0f);
	color = vec4(reflectcolor,1.0f);
	//color = Snormal_color;
	//color = vec4(Vec3Film(LightMap.Specular_map), 1.0f);
};