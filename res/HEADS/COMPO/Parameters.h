#pragma once
#include "support.h"
#include "glm/glm.hpp"
#include <unordered_map>
#include "glm/glm.hpp"

enum ParaType
{
	NONE_PARA, FLOAT_PARA, INT_PARA, STRING_PARA, VEC2_PARA, VEC3_PARA, VEC4_PARA
};

class Parameters
{
public:
	Parameters();
	~Parameters();

	bool is_para_changed;
	unsigned int para_uniformID = 0;
	std::string para_name;
	ParaType para_type;

	Parameters GenParaItem(ParaType type);
};

namespace Para {

	class Float : public Parameters {
		float para = 0.0f;
		float* GetParaPtr() { return &para; }
		float GetPara() const { return para; }

		Float() { para_type = FLOAT_PARA; }
	};

	class Int : public Parameters {
		int para = 0;
		int* GetParaPtr() { return &para; }
		int GetPara() const { return para; }

		Int() { para_type = INT_PARA; }
	};

	class String : public Parameters {
		std::string para = "";
		std::string* GetParaPtr() { return &para; }
		std::string GetPara() const { return para; }

		String() { para_type = STRING_PARA; }
	};

	class Vec2 : public Parameters {
		glm::vec2 para = glm::vec2(0, 0);
		glm::vec2* GetParaPtr() { return &para; }
		glm::vec2 GetPara() const { return para; }

		Vec2() { para_type = VEC2_PARA; }
	};

	class Vec3 : public Parameters {
		glm::vec3 para = glm::vec3(0, 0, 0);
		glm::vec3* GetParaPtr() { return &para; }
		glm::vec3 GetPara() const { return para; }

		Vec3() { para_type = VEC3_PARA; }
	};

	class Vec4 : public Parameters {
		glm::vec4 para = glm::vec4(0, 0, 0, 0);
		glm::vec4* GetParaPtr() { return &para; }
		glm::vec4 GetPara() const { return para; }

		Vec4() { para_type = VEC4_PARA; }
	};
}
