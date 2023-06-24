#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 U_model;
uniform vec3 U_offset;

void main()
{
    gl_Position = U_model * vec4(position, 1.0) + vec4(-U_offset, 0);
}  