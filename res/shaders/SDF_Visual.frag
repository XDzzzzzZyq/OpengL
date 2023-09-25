#version 430 core

layout(location = 0) out vec4 EMIScolor;
layout(location = 1) out vec4 POScolor;
layout(location = 2) out vec4 NORMALcolor;
layout(location = 3) out vec4 ALBEDOcolor;
layout(location = 4) out vec4 MRSEcolor;
layout(location = 5) out vec4 RANDcolor;
layout(location = 6) out vec4 IDcolor;
layout(location = 7) out vec4 MASKcolor;

in vec2 uv;
in vec3 pix_pos;
in vec4 normal_color;
in vec4 Snormal_color;

uniform vec3 ID_color;
uniform vec3 RAND_color;
uniform int is_selected;

layout(std430, binding = 0) buffer SDF {
	float SDF_data[];
};

void main() {

	EMIScolor = vec4(vec3(1), 1);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	NORMALcolor = normal_color;
	POScolor = vec4(pix_pos, 1);
	MASKcolor = vec4(1, is_selected, 0, 1);
	ALBEDOcolor = vec4(vec3(0), 1);
	MRSEcolor = vec4(0, 1, 1, 1);

};