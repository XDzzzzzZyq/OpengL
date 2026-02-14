#pragma once

#include "Controllers.h"
#include "buffer/FrameBuffer.h"

class ViewportController : public Controllers
{
private:
	FrameBuffer* viewport_fb{ nullptr };
public:
	virtual void bind(EventPool& pool) override;
};

