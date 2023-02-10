#version 430 core

layout(location = 0) out vec4 color;

uniform sampler2D U_color;

in vec2 screen_uv;

void main(){
	color = texture(U_color, screen_uv);
}