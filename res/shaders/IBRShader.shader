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
vec4 outline;

uniform sampler2D screen_texture;
uniform sampler2D id_texture;
uniform sampler2D hdr_texture;
uniform vec3 pure_color;

uniform mat4 cam_rotM;
uniform float cam_fov;
uniform float cam_ratio;
uniform vec3 ID_color;
uniform vec3 RAND_color;

uniform float activeID;

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

vec4 GetSelect(vec4 col, float act){
	col[3] = 1.0f;
	col -= vec4((act+1)/256, 0, 0, 0);
	if(col[0]>=0 || col[0]<=-0.005){
		col = vec4(1,1,1,1);
	}
	return col;
}

vec4 Conv3_3(sampler2D tex, ivec2 res, vec2 uv, int rad, float act){
	vec4 result = vec4(0.0f);
	vec2 offest = 3*vec2(1) / res;
	for(int i = -rad; i<=rad; i++){
		for(int j = -rad; j<=rad; j++){
			result += GetSelect(texture(tex, uv + offest * vec2( i , j )), act);
		}
	}
	//result  = vec4(4*rad*rad)-result;
	result[3] = 4*rad*rad;
	return result/(4*rad*rad);
}

void main(){		 
	reUV  = reMapUV(screen_uv);
	ray_dir =  cam_rotM * vec4(vec3(-1,-1,1)*normalize( vec3(reUV * tan(cam_fov/2) * vec2(cam_ratio, 1) ,1)  ) ,1);

	screen_color = texture(screen_texture,screen_uv);
	hdr_color = vec4(texture(hdr_texture,genHdrUV(normalize(vec3(ray_dir)))).rgb,1.0f);

	color =  hdr_color * (1 - screen_color[3]) + vec4(vec3(screen_color),1.0f) * screen_color[3];

	if(activeID != 0){
		IDcolor = texture(id_texture, screen_uv);
		IDcolor[3] = 1.0f;
		IDcolor = GetSelect(IDcolor, activeID);
		outline = (vec4(1,1,1,2)-IDcolor) * Conv3_3(id_texture, textureSize(id_texture, 0), screen_uv, 1, activeID);
		if(outline[0]>0.1){
			color = vec4(1,1,1,1);
		}	
	}

	//color = Conv3_3(id_texture, div, screen_uv, 3);
	IDcolor = vec4(ID_color, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};