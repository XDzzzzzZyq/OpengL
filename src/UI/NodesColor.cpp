#include "Nodes.h"

std::vector<ImU32> Nodes::n_color_list = {
	IM_COL32(197,105,0,255),
	IM_COL32(30,170,0,255)
};

std::vector<ImU32> Nodes::pin_color_list = {
	IM_COL32(100,100,100,255),											 //FLOAT_PARA, 
	IM_COL32(120,120,120,255),											 //INT_PARA, 
	IM_COL32(0,178,105,255),											 //BOOL_PARA, 
	IM_COL32(30,170,0 ,255),											 //STRING_PARA, 
	IM_COL32(30,170,70,255),											 //VEC2_PARA, 
	IM_COL32(0 ,68,203,255),											 //VEC3_PARA, 
	IM_COL32(153,39,202,255),											 //VEC4_PARA, 
	IM_COL32(30,170,70,255),											 //MAT3_PARA, 
	IM_COL32(30,170,70,255),											 //MAT4_PARA, 
	IM_COL32(30,170,70,255),											 //TEXTURE_PARA
	IM_COL32(30,170,70,255),											 //
	IM_COL32(30,170,70,255),											 //
};