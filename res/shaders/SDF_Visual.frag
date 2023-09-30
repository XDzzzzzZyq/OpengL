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

layout(std430, binding = 6) buffer SDF {
	vec3 SDF_pos;
	vec3 SDF_scale;
	vec3 SDF_size;
	int SDF_subdiv;
	uint SDF_data[];
};

// This shader partially refered to the Repo: https://github.com/GPUOpen-Effects/TressFX/tree/master

float LinearInterpolate(float a, float b, float t)
{
    return a * (1.0f - t) + b * t;
}

//    bilinear interpolation
//
//         p    :  1-p
//     c------------+----d
//     |            |    |
//     |            |    |
//     |       1-q  |    |
//     |            |    |
//     |            x    |
//     |            |    |
//     |         q  |    |
//     a------------+----b
//         p    :  1-p
//
//    x = BilinearInterpolate(a, b, c, d, p, q)
//      = LinearInterpolate(LinearInterpolate(a, b, p), LinearInterpolate(c, d, p), q)
float BilinearInterpolate(float a, float b, float c, float d, float p, float q)
{
    return LinearInterpolate( LinearInterpolate(a, b, p), LinearInterpolate(c, d, p), q );
}

//    trilinear interpolation
//
//                      c        p            1-p    d
//                       ------------------+----------
//                      /|                 |        /|
//                     /                   |       / |
//                    /  |                 |1-q   /  |
//                   /                     |     /   |
//                  /    |                 |    /    |
//               g ------------------+---------- h   |
//                 |     |           |     |   |     |
//                 |                 |     +   |     |
//                 |     |           |   r/|   |     |
//                 |                 |   / |q  |     |
//                 |     |           |  x  |   |     |
//                 |   a - - - - - - | / - + - |- - -| b
//                 |    /            |/1-r     |     /
//                 |                 +         |    /
//                 |  /              |         |   /
//                 |                 |         |  /
//                 |/                |         | /
//                 ------------------+----------
//              e                            f
//
//		x = TrilinearInterpolate(a, b, c, d, e, f, g, h, p, q, r)
//		  = LinearInterpolate(BilinearInterpolate(a, b, c, d, p, q), BilinearInterpolate(e, f, g, h, p, q), r)
float TrilinearInterpolate(float a, float b, float c, float d, float e, float f, float g, float h, float p, float q, float r)
{
    return LinearInterpolate(BilinearInterpolate(a, b, c, d, p, q), BilinearInterpolate(e, f, g, h, p, q), r);
}

uint FloatFlip3(float fl)
{
    uint f = floatBitsToUint(fl);
    return (f << 1) | (f >> 31);		//Rotate sign bit to least significant
}
float IFloatFlip3(uint f2)
{
    uint u = (f2 >> 1) | (f2 << 31);
	return uintBitsToFloat(u);
}

float ReadSDFAt(ivec3 index){
	int index_linear = int(index.x + index.y * SDF_size.x + index.z * SDF_size.x * SDF_size.y);
	return IFloatFlip3(SDF_data[index_linear]);
}

float ReadSDF(vec3 pos){
	vec3 transf = (pos - SDF_pos) / SDF_scale;  // normalize
	vec3 over = transf - clamp(transf, -1, 1);  // over the field
	transf = clamp(transf, -1, 1) + 1;			// [0, 2]
	transf *= 0.5;								// [0, 1]
	transf *= (SDF_size - 1);					// [(0,0,0), (W,D,H)]


	float distance_000 = ReadSDFAt(ivec3(transf) + ivec3(0,0,0));
	float distance_001 = ReadSDFAt(ivec3(transf) + ivec3(0,0,1));
	float distance_010 = ReadSDFAt(ivec3(transf) + ivec3(0,1,0));
	float distance_011 = ReadSDFAt(ivec3(transf) + ivec3(0,1,1));
	float distance_100 = ReadSDFAt(ivec3(transf) + ivec3(1,0,0));
	float distance_101 = ReadSDFAt(ivec3(transf) + ivec3(1,0,1));
	float distance_110 = ReadSDFAt(ivec3(transf) + ivec3(1,1,0));
	float distance_111 = ReadSDFAt(ivec3(transf) + ivec3(1,1,1));

	vec3 interp = fract(transf);

	return TrilinearInterpolate(distance_000, distance_100, distance_010, distance_110,
                                distance_001, distance_101, distance_011, distance_111,
                                interp.x, interp.y, interp.z) + length(over);
}

void main() {

	float sdf = ReadSDF(pix_pos);

	EMIScolor = vec4(vec3(sdf), 1);
	IDcolor = vec4(ID_color / 256, 1.0f);
	RANDcolor = vec4(RAND_color, 1.0f);
	NORMALcolor = normal_color;
	POScolor = vec4(pix_pos, 1);
	MASKcolor = vec4(1, is_selected, 0, 1);
	ALBEDOcolor = vec4(vec3(0), 1);
	MRSEcolor = vec4(0, 1, 1, 1);

};