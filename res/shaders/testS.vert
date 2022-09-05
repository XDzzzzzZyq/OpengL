#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvIn;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 smo_normal;

out vec2 uv;
out vec4 testcolor;
out vec3 pix_pos;
out vec4 normal_color;
out vec4 Snormal_color;

uniform mat4 U_cam_trans;
uniform mat4 U_obj_trans;
uniform mat4 U_ProjectM;
uniform float z_inp;

void main() {
	uv = uvIn;

	gl_Position = U_ProjectM * U_cam_trans * U_obj_trans * vec4(position, 1.0f);
	testcolor = vec4(position, 1.0f);

	Snormal_color = vec4(normalize(mat3(U_obj_trans) * smo_normal), 1.0f);
	normal_color = vec4(normalize(mat3(U_obj_trans) * normal), 1.0f);
	pix_pos = vec3(U_obj_trans * vec4(position, 1.0f));
	//gl_Position = U_ProjectM * position;
};