#include "ViewportController.h"

#include "xdz_math.h"

#include "events/ViewportEvents.h"
#include "events/EditorEvents.h"

int GetSelectID(const FrameBuffer* info_fb, GLuint x, GLuint y)
{
	if (-glm::vec2(5, 5) < glm::vec2(x, y) && glm::vec2(x, y) < info_fb->GetFrameBufferSize() * glm::vec2(1, 2))
		//return GetActiveEnvironment()->envir_frameBuffer->ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
		return info_fb->ReadPix(x, y, ID_FB).GetID();
	else
		return -1;
}

void ViewportController::bind(EventPool& pool)
{
	pool.subscribe<ViewportSelectedEvent>([this, &pool](ViewportSelectedEvent e) {
		const int id = GetSelectID(viewport_fb, e.pix_x, e.pix_y);
		pool.emit(ObjectSelectedEvent{ id, e.increament });
		});

	pool.subscribe<ViewportResizeEvent>([this, &pool](const ViewportResizeEvent& e) {
		// only forward the event to editor
		pool.emit(RenderSurfaceResizedEvent{ e.width, e.height });
		});

	pool.subscribe<FrameBufferResetEvent>([this, &pool](const FrameBufferResetEvent& e) {
		FrameBuffer* fb_ptr = static_cast<FrameBuffer*>(e.buffer_obj);
		viewport_fb = fb_ptr;

		const GLuint tex_id = fb_ptr->GetFBTextureID(COMBINE_FB);
		pool.emit(ViewportImageResetEvent{ tex_id });
		});
}
