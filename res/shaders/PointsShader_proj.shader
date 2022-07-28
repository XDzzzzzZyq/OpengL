#shader vertex
#version 330 core

layout(location = 0) in vec3 position;


uniform vec3 pos_list[6];
uniform vec3 point_color;
out vec3 L_color;

uniform mat4 U_cam_trans;
mat4 post_cam_trans;

uniform mat4 U_ProjectM;
uniform float U_Scale;

void main(){
	L_color = point_color;

	post_cam_trans = U_Scale * transpose( mat4(mat3(U_cam_trans)) );

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(pos_list, 1.0f) + vec4(pos_list[gl_InstanceID], 0.0f)) - vec4(0,0,0.01f,0);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 L_color;

uniform float SpiritOpacity;

void main(){		   
						   
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(L_color, SpiritOpacity);
	//color = L_color;
	//color = uvcolor;
	//color = vec4(1.0f,1.0f,1.0f,1.0f);
};