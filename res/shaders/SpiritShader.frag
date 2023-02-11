#version 330 core

layout(location = 0) out vec4 COMBcolor;
layout(location = 5) out vec4 RANDcolor;
layout(location = 6) out vec4 IDcolor;

in vec2 uv;
in vec3 L_color;

uniform sampler2D U_Texture;
uniform float SpiritOpacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main() {

	vec4 uvcolor = texture(U_Texture, uv);
	//color = vec4(uv,0.0f,1.0f);
	COMBcolor = vec4(L_color, vec3(uvcolor)[0] * SpiritOpacity);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};