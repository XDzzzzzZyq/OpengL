#pragma once

#include "Controllers.h"
class ShaderController : public Controllers
{
public:
	virtual void bind(EventPool& pool) override;
};

