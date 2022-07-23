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

uniform sampler2D IBRtexture;

void main(){		   
						  
	//color = vec4(uv,0.0f,1.0f);
	//color = 1 - texture(IBRtexture,screen_uv);
	color = vec4(1.0f,1.0f,0.0f,1.0f);
};