#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aScreen_uv;

out vec2 screen_uv;

void main() {

	screen_uv = aScreen_uv;
	gl_Position = vec4(aPos, 0.0f, 1.0f);

};
