#version 430 core

layout(location = 0) in vec3 position;
layout(std430, binding = 3) buffer pos_buffer {
	float pos_list[];
};

uniform int pos_count;

uniform vec3 point_color;
out vec3 L_color;

uniform mat4 U_cam_trans;
mat4 post_cam_trans;

uniform mat4 U_ProjectM;
uniform float U_Scale;

out vec4 inst_pos;

void main() {
	L_color = point_color;
	inst_pos = vec4(pos_list[3 * gl_InstanceID], pos_list[3 * gl_InstanceID + 1], pos_list[3 * gl_InstanceID + 2], 0.0f);

	post_cam_trans = transpose(mat4(mat3(U_cam_trans)));

	gl_Position = U_ProjectM * U_cam_trans * (post_cam_trans * vec4(0.1 * U_Scale * position, 1.0f) + inst_pos);

};