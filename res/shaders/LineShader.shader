#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 U_cam_trans;
uniform mat4 U_ProjectM;
uniform mat4 U_Trans;

void main(){

	gl_Position = U_ProjectM * U_cam_trans * U_Trans * vec4(position, 1.0f) - vec4(0,0,0.01f,0);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform float dlineOpacity;
uniform vec3 U_color;

void main(){		   
						  
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(U_color, dlineOpacity);
};