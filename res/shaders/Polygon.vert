#version 330 core

layout(location = 0) in vec2 position;

out vec3 pix_pos;
out vec4 normal_color;

uniform mat4 U_cam_trans;
uniform mat4 U_obj_trans;
uniform mat4 U_ProjectM;

void main() {
	gl_Position = U_ProjectM * U_cam_trans * U_obj_trans * vec4(position, 0.0f, 1.0f);
	pix_pos = vec3(gl_Position);
	normal_color = U_ProjectM * U_cam_trans * U_obj_trans * vec4(0, 0, 1.0f, 0.0f);
};
