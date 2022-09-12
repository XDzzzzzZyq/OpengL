#include "ShaderLib.h"

S_func ShaderLib::FilmicF;
S_func ShaderLib::FilmicV4;

S_const ShaderLib::PI = { FLOAT_PARA, "B_PI", "3.141592653589", {} };
S_const ShaderLib::Pix_UV_ratio = { FLOAT_PARA, "B_PIX_UV_RATIO", "1.0 / textureSize(select_texture, 0)", {}};