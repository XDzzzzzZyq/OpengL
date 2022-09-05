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

void main() {
	L_pos = vec3(Light_data[0], Light_data[1], Light_data[2]);
	L_color = vec3(Light_data[3], Light_data[4], Light_data[5]);
	uv = uvIn;

	post_cam_trans = transpose(mat4(mat3(U_cam_trans)));

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(U_Scale * position, 1.0f) + vec4(L_pos, 0.0f));

};