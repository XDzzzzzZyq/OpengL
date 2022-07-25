#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aScreen_uv;

out vec2 screen_uv;

void main(){
	
	screen_uv = aScreen_uv;
	gl_Position = vec4(aPos,0.0f,1.0f);
	
};








#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 screen_uv;
vec4 screen_color;
vec4 hdr_color;

uniform sampler2D screen_texture;
uniform sampler2D hdr_texture;
uniform vec3 pure_color;

void main(){		   
	screen_color = texture(screen_texture,screen_uv);
	hdr_color = texture(hdr_texture,screen_uv);
	color =  vec4(1.0f,1.0f,0.0f,1.0f) * (1 - screen_color[3]) + screen_color * screen_color[3];
	//color = vec4(screen_color[3],screen_color[3],screen_color[3],1.0f);
	//color = texture(IBRtexture,screen_uv);
	//color = vec4(1.0f,1.0f,0.0f,1.0f);
};