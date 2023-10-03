#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvIn;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 smo_normal;

uniform mat4 U_model;
uniform vec3 U_offset;

out vec3 normal_color;
out vec3 Snormal_color;

void main()
{
    gl_Position = U_model * vec4(position, 1.0) + vec4(-U_offset, 0);
    Snormal_color = normalize(mat3(U_model) * smo_normal);
	normal_color = normalize(mat3(U_model) * normal);
}  