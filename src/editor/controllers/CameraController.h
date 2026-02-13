#pragma once

#include "Controllers.h"

class CameraController : public Controllers
{
public:
	virtual void bind(EventPool& pool) override;
};

