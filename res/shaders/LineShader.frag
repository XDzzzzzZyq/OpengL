#version 330 core

layout(location = 0) out vec4 COMBcolor;
layout(location = 1) out vec4 POScolor;
layout(location = 2) out vec4 RANDcolor;
layout(location = 3) out vec4 IDcolor;
layout(location = 4) out vec4 ALPHAcolor;
layout(location = 5) out vec4 SELECcolor;
layout(location = 6) out vec4 NORMALcolor;

uniform float dlineOpacity;
uniform vec3 U_color;
uniform vec3 ID_color;
uniform vec3 RAND_color;
uniform int is_selected;

void main() {

	COMBcolor = vec4(U_color, dlineOpacity);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	SELECcolor = vec4(is_selected);
	ALPHAcolor = vec4(1);
};