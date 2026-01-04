#version 330 core
layout(location = 0) out vec4 shadowmap;

void main()
{             
    float lightDistance = gl_FragCoord.z;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;

    // calculate d^n
    float d = lightDistance;
    float d2 = d*d;
    shadowmap = vec4(d, d2, d2*d, d2*d2);
} 