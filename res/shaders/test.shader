#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uvIn;
layout(location = 2) in vec3 normal;

out vec2 uv;

uniform mat4 U_ProjectM;

void main() {
	uv = uvIn;
	gl_Position = U_ProjectM * position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 uv;

uniform vec4 U_color;
uniform sampler2D U_Texture;
uniform float is;

void main() {
	//color = U_color;
	vec4 uvcolor = texture(U_Texture, uv);
	vec4 solidcolor = U_color;
	color = is * uvcolor + (1 - is) * solidcolor;
};