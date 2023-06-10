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
in float dist;

uniform sampler2D U_Texture;
uniform float SpiritOpacity;
uniform vec3 ID_color;
uniform vec3 RAND_color;

void main() {
	vec4 spirit_color = texture(U_Texture, uv);
	float range = smoothstep(0, 100, dist)*0.3+0.05;
	float alpha = smoothstep(0.5-range, 0.5+range, spirit_color.r);
	float bound = distance(uv, vec2(0.5)) < 0.5 ? 1 : 0;
	//color = vec4(uv,0.0f,1.0f);
	EMIScolor = vec4(L_color, alpha);
	IDcolor = vec4(ID_color / 256, bound);
	RANDcolor = vec4(RAND_color, bound);
	MRSEcolor = vec4(0, 0, 0, alpha);
	MASKcolor = vec4(spirit_color.r * SpiritOpacity, 0, 0, alpha);
};
