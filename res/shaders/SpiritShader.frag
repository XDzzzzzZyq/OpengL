#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 POScolor;
layout(location = 2) out vec4 IDcolor;
layout(location = 3) out vec4 RANDcolor;
layout(location = 4) out vec4 SELECcolor;

in vec2 uv;
in vec3 L_color;

uniform sampler2D U_Texture;
uniform float SpiritOpacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main() {

	vec4 uvcolor = texture(U_Texture, uv);
	//color = vec4(uv,0.0f,1.0f);
	color = vec4(L_color, vec3(uvcolor)[0] * SpiritOpacity);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
};