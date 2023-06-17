#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 U_lightproj;
uniform mat4 U_model;

void main()
{
    gl_Position = U_lightproj * U_model * vec4(position, 1.0);
}  