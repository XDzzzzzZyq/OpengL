#include "Environment.h"

Environment::Environment(const std::string& texpath, GLuint slot)
{
	envir_type = EnvironmentType::TEXTURE_ENVIR;
	envir_HDR = Texture(texpath, GL_REPEAT);
	envir_HDR.Bind(slot);
	envir_HDR.Tex_slot = slot;
}

Environment::Environment()
{

}

Environment::~Environment()
{

}

void Environment::ChangeEnvirTexture(const std::string& texpath) const
{

}

void Environment::ChangeEnvirType(const EnvironmentType& type) const
{

}

void Environment::GenFloatData() const
{

}
