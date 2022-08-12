#shader vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(std430, binding = 3) buffer pos_buffer{
	float pos_list[];
};

uniform int pos_count;

uniform vec3 point_color;
out vec3 L_color;

uniform vec3 testList[8];

uniform mat4 U_cam_trans;
mat4 post_cam_trans;

uniform mat4 U_ProjectM;
uniform float U_Scale;

vec4 inst_pos;

void main(){
	L_color = point_color;
	inst_pos = vec4(pos_list[3 * gl_InstanceID], pos_list[3 * gl_InstanceID  +1], pos_list[3 * gl_InstanceID + 2], 0.0f);

	post_cam_trans = transpose( mat4(mat3(U_cam_trans)) );

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(0.1 * U_Scale * position, 1.0f) + inst_pos) - vec4(0,0,0.01f,0);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;
layout(location = 3) out float ALPHA;

in vec3 L_color;

uniform float U_Opacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main(){		   
						   
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(L_color, U_Opacity);
	IDcolor=vec4(ID_color/10, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};