#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

// This shader partially refered to the Repo: https://github.com/GPUOpen-Effects/TressFX/tree/master

layout(std430, binding = 6) buffer SDF {
	vec3 SDF_pos;
	vec3 SDF_scale;
	vec3 SDF_size;
	int SDF_subdiv;
	uint SDF_data[];
};

vec3 GetPosFromIndex(int x, int y, int z){
	vec3 center = 0.5f * SDF_size - 0.5f;
	vec3 pos_raw = vec3(x, y, z) - center;
	pos_raw /= length(center);

	return pos_raw * SDF_scale + SDF_pos;
}

float DistancePointToEdge(vec3 p, vec3 x0, vec3 x1, out vec3 n)
{
    vec3 x10 = x1 - x0;

    float t = dot(x1 - p, x10) / dot(x10, x10);
    t = max(0.0f, min(t, 1.0f));

    vec3 a = p - (t*x0 + (1.0f - t)*x1);
    float d = length(a);
    n = a / (d + 1e-30f);

    return d;
}

float SignedDistancePointToTriangle(vec3 p, vec3 x0, vec3 x1, vec3 x2)
{
    float d = 0;
    vec3 x02 = x0 - x2;
    float l0 = length(x02) + 1e-30f;
    x02 = x02 / l0;
    vec3 x12 = x1 - x2;
    float l1 = dot(x12, x02);
    x12 = x12 - l1*x02;
    float l2 = length(x12) + 1e-30f;
    x12 = x12 / l2;
    vec3 px2 = p - x2;

    float b = dot(x12, px2) / l2;
    float a = (dot(x02, px2) - l1*b) / l0;
    float c = 1 - a - b;

    // normal vector of triangle. Don't need to normalize this yet.
    vec3 nTri = cross((x1 - x0), (x2 - x0));
    vec3 n;

    float tol = 1e-8f;

    if (a >= -tol && b >= -tol && c >= -tol)
    {
        n = p - (a*x0 + b*x1 + c*x2);
        d = length(n);

        vec3 n1 = n / d;
        vec3 n2 = nTri / (length(nTri) + 1e-30f);		// if d == 0

        n = (d > 0) ? n1 : n2;
    }
    else
    {
        vec3 n_12;
        vec3 n_02;
        d = DistancePointToEdge(p, x0, x1, n);

        float d12 = DistancePointToEdge(p, x1, x2, n_12);
        float d02 = DistancePointToEdge(p, x0, x2, n_02);

        d = min(d, d12);
        d = min(d, d02);

        n = (d == d12) ? n_12 : n;
        n = (d == d02) ? n_02 : n;
    }

    d = (dot(p - x0, nTri) < 0.f) ? -d : d;

    return d;
}

// the sign of distance should not be involved in atomicMin(), 
// then swap it to the last digit

// [ sign ] [ -------- float --------- ] => [ -------- float --------- ] [ sign ]

uint FloatFlip3(float fl)
{
    uint f = floatBitsToUint(fl);
    return (f << 1) | (f >> 31);
}
uint IFloatFlip3(uint f2)
{
    return (f2 >> 1) | (f2 << 31);
}

void main()
{
	for(int z = 0; z < SDF_size.z; z++){
		for(int y = 0; y < SDF_size.y; y++){
			for(int x = 0; x < SDF_size.x; x++){
				int index = int(x + y * SDF_size.x + z * SDF_size.y * SDF_size.x);
				vec3 sdf_pos = GetPosFromIndex(x,y,z);
				float dist = SignedDistancePointToTriangle(sdf_pos, vec3(gl_in[0].gl_Position), vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position));
                atomicMin(SDF_data[index], FloatFlip3(dist));
			}
		}
	}
} 