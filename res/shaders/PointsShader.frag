#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 IDcolor;
layout(location = 2) out vec4 RANDcolor;
layout(location = 3) out vec4 SELECcolor;

in vec3 L_color;

uniform float U_Opacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;
uniform int is_selected;

void main() {

	//color = vec4(uv,0.0f,1.0f);
	color = vec4(L_color, U_Opacity);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	SELECcolor = vec4(is_selected);
};