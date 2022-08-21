#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 U_cam_trans;
uniform mat4 U_ProjectM;
uniform mat4 U_Trans;

void main(){

	gl_Position = U_ProjectM * U_cam_trans * U_Trans * vec4(position, 1.0f);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;
layout(location = 3) out vec4 SELECcolor;

uniform float dlineOpacity;
uniform vec3 U_color;
uniform vec3 ID_color;
uniform vec3 RAND_color;
uniform int is_selected;

void main(){		   
						  
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(U_color, dlineOpacity);
	IDcolor = vec4(ID_color/256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	SELECcolor = vec4(is_selected);
};