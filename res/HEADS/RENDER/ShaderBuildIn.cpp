#include "ShaderLib.h"

/*			Build-in functions			*/

const S_func ShaderLib::FilmicF;
const S_func ShaderLib::FilmicV4;

/*			Build-in constants			*/

const S_const ShaderLib::PI = { FLOAT_PARA, "B_PI", "3.141592653589", {} };
const S_const ShaderLib::Pix_UV_ratio = { FLOAT_PARA, "B_PIX_UV_RATIO", "1.0 / textureSize(select_texture, 0)", {} };

/*		Build-in constexpressions		*/

const S_const ShaderLib::v_albedo = { VEC3_PARA,  "m_albedo", "U_albedo", {} };
const S_const ShaderLib::v_metal  = { FLOAT_PARA, "m_metal",  "U_metal", {} };
const S_const ShaderLib::v_rough  = { FLOAT_PARA, "m_rough",  "U_rough", {} };
const S_const ShaderLib::v_specu  = { FLOAT_PARA, "m_specu",  "U_specu", {} };
const S_const ShaderLib::v_emis_c = { VEC3_PARA,  "m_emis_c", "U_emis_c", {} };
const S_const ShaderLib::v_emis_s = { FLOAT_PARA, "m_emis_s", "U_emis_s", {} };
const S_const ShaderLib::v_alpha  = { FLOAT_PARA, "m_alpha",  "U_alpha", {} };

const S_const ShaderLib::t_albedo = { VEC3_PARA,  "m_albedo", "texture2D(U_albedo, uv).rgb", {} };
const S_const ShaderLib::t_metal  = { FLOAT_PARA, "m_metal",  "texture2D(U_metal, uv).r", {} };
const S_const ShaderLib::t_rough  = { FLOAT_PARA, "m_rough",  "texture2D(U_rough, uv).r", {} };
const S_const ShaderLib::t_specu  = { FLOAT_PARA, "m_specu",  "texture2D(U_specu, uv).r", {} };
const S_const ShaderLib::t_emis_c = { VEC3_PARA,  "m_emis_c", "texture2D(U_emis_c, uv).rgb", {} };
const S_const ShaderLib::t_emis_s = { FLOAT_PARA, "m_emis_s", "texture2D(U_emis_s, uv).r", {} };
const S_const ShaderLib::t_alpha  = { FLOAT_PARA, "m_alpha",  "texture2D(U_alpha, uv).r", {} };

const std::vector<S_const> ShaderLib::v_material = { v_albedo,v_metal,v_rough,v_specu,v_emis_c,v_emis_s,v_alpha };
const std::vector<S_const> ShaderLib::t_material = { t_albedo,t_metal,t_rough,t_specu,t_emis_c,t_emis_s,t_alpha };