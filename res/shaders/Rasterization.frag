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

uniform sampler2D U_albedo;
uniform float U_metal;
uniform float U_rough;
uniform float U_specu;
uniform vec3 U_emis_c;
uniform float U_emis_s;
uniform float U_alpha;
uniform vec3 ID_color;
uniform vec3 RAND_color;
uniform int is_selected;

const vec3 m_albedo = texture2D(U_albedo, uv).rgb;
const float m_metal = U_metal;
const float m_rough = U_rough;
const float m_specu = U_specu;
const vec3 m_emis_c = U_emis_c;
const float m_emis_s = U_emis_s;
const float m_alpha = 1;

void main() {

	EMIScolor = vec4(m_emis_c, 1);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	NORMALcolor = normal_color;
	POScolor = vec4(pix_pos, 1);
	MASKcolor = vec4(1, is_selected, 0, 1);
	ALBEDOcolor = vec4(m_albedo, 1);
	MRSEcolor = vec4(m_metal, m_rough, m_specu, m_emis_s);

};