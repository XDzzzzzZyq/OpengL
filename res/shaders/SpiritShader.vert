#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvIn;

out vec2 uv;
out float dist;
out vec3 L_color;

uniform vec3 U_pos;
uniform vec3 U_col;

uniform mat4 U_cam_trans;
uniform mat4 U_ProjectM;
uniform float U_Scale;

void main() {
	uv = 1-uvIn;

	mat4 post_cam_trans = transpose(mat4(mat3(U_cam_trans)));
	dist = distance(U_pos, vec3(U_cam_trans*vec4(vec3(0),-1)));
	L_color = U_col;

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(U_Scale * position, 1.0f) + vec4(U_pos, 0.0f));
};