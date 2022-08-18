#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvIn;

out vec2 uv;

uniform float Light_data[6];
vec3 L_pos;
out vec3 L_color;

uniform mat4 U_cam_trans;
mat4 post_cam_trans;

uniform mat4 U_ProjectM;
uniform float U_Scale;

void main(){
	L_pos = vec3(Light_data[0], Light_data[1], Light_data[2]);
	L_color = vec3(Light_data[3], Light_data[4], Light_data[5]);
	uv = uvIn;

	post_cam_trans = transpose( mat4(mat3(U_cam_trans)) );

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(U_Scale * position, 1.0f) + vec4(L_pos, 0.0f)) - vec4(0,0,0.01f,0);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;

in vec2 uv;
in vec3 L_color;

uniform sampler2D U_Texture;
uniform float SpiritOpacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main(){		   
						   
	vec4 uvcolor = texture(U_Texture,uv);
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(L_color, vec3(uvcolor)[0]*SpiritOpacity);
	IDcolor=vec4(ID_color/256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};