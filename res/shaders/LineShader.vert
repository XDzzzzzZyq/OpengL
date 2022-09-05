#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 U_cam_trans;
uniform mat4 U_ProjectM;
uniform mat4 U_Trans;

void main() {

	gl_Position = U_ProjectM * U_cam_trans * U_Trans * vec4(position, 1.0f);

};