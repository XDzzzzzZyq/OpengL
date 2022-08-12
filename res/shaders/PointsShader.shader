#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform int pos_count;

uniform vec3 point_color;
out vec3 L_color;

uniform vec3 testList[8];

uniform mat4 U_cam_trans;
mat4 post_cam_trans;

uniform mat4 U_ProjectM;
uniform float U_Scale;

void main(){
	L_color = point_color;

	post_cam_trans = 0.1 * U_Scale * transpose( mat4(mat3(U_cam_trans)) );

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(position, 1.0f) + vec4(testList[gl_InstanceID], 0.0f)) - vec4(0,0,0.01f,0);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;

in vec3 L_color;

uniform float U_Opacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main(){		   
						   
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(L_color, U_Opacity);
	IDcolor = vec4(ID_color, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};