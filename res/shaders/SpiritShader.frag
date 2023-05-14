#version 330 core

layout(location = 0) out vec4 EMIScolor;
layout(location = 1) out vec4 POScolor;
layout(location = 2) out vec4 NORMALcolor;
layout(location = 3) out vec4 ALBEDOcolor;
layout(location = 4) out vec4 MRSEcolor;
layout(location = 5) out vec4 RANDcolor;
layout(location = 6) out vec4 IDcolor;
layout(location = 7) out vec4 MASKcolor;

in vec2 uv;
in vec3 L_color;

uniform sampler2D U_Texture;
uniform float SpiritOpacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main() {

	vec4 spirit_color = texture(U_Texture, uv);
	//color = vec4(uv,0.0f,1.0f);
	EMIScolor = vec4(L_color, spirit_color.r);
	IDcolor = vec4(ID_color / 256, 1);
	RANDcolor = vec4(RAND_color, 1);
	MRSEcolor = vec4(0, 0, 0, spirit_color.r);
	MASKcolor = vec4(spirit_color.r * SpiritOpacity, 0, 0, spirit_color.r);
};