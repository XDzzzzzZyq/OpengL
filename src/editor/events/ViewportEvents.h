#pragma once

struct ViewportSelectedEvent
{
	int pix_x, pix_y; // converted by UI layer
	bool increament;
};

struct ViewportResizeEvent
{
	int width, height;
};

struct ViewportImageResetEvent
{
	unsigned int tex_id;
};