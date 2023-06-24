#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 U_model;
uniform mat4 U_trans;
uniform vec2 U_UV;
uniform float ratio;

void main()
{
    vec3 U_offset = vec3(U_trans*vec4(2 * U_UV * vec2(ratio, 1) - 1, 0, 1));
    gl_Position = U_model * vec4(position, 1.0) + vec4(-U_offset, 0);
}  