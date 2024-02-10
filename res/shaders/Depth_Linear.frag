#version 330 core
layout(location = 0) out vec4 shadowmap;

in vec4 FragPos;

uniform float far_plane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;

    // calculate d^n
    float d = lightDistance;
    float d2 = d*d;
    shadowmap = vec4(d, d2, d2*d, d2*d2);
}  