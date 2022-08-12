#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aScreen_uv;

out vec2 screen_uv;

void main(){
	
	screen_uv = aScreen_uv;
	gl_Position = vec4(aPos,0.0f,1.0f);
	
};








#shader fragment
#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;

in vec2 screen_uv;
vec4 screen_color;
vec4 hdr_color;

uniform sampler2D screen_texture;
uniform sampler2D hdr_texture;
uniform vec3 pure_color;

uniform mat4 cam_rotM;
uniform float cam_fov;
uniform float cam_ratio;
uniform vec3 ID_color;
uniform vec3 RAND_color;

vec4 ray_dir;
vec2 reUV;
vec2 equirectangularUV;

vec2 genHdrUV(vec3 dir){
	float spin = atan(dir[2]/dir[0]);
	float elevation = atan(dir[1]/sqrt(dir[0]*dir[0]+dir[2]*dir[2]));
	if(dir[0]>0){
		spin-=3.1415926f;
	}
	return vec2(((spin/3.1415926f)/2)+0.5, (elevation/-3.1415926f+1)/2);
}

vec2 reMapUV(vec2 uv){
	return uv*2-vec2(1.0f,1.0f);
}

void main(){		 
	reUV  = reMapUV(screen_uv);
	ray_dir =  cam_rotM * vec4(vec3(-1,-1,1)*normalize( vec3(reUV * tan(cam_fov/2) * vec2(cam_ratio, 1) ,1)  ) ,1);

	screen_color = texture(screen_texture,screen_uv);
	hdr_color = vec4(texture(hdr_texture,genHdrUV(normalize(vec3(ray_dir)))).rgb,1.0f);

	color =  hdr_color * (1 - screen_color[3]) + screen_color * screen_color[3];
	IDcolor = vec4(ID_color, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};