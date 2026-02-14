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
		const int id = GetSelectID(id_fb, e.pix_x, e.pix_y);
		pool.emit(ObjectSelectedEvent{ id, e.increament });
		});

	pool.subscribe<ViewportResizeEvent>([this, &pool](const ViewportResizeEvent& e) {
		// only forward the event to editor
		pool.emit(RenderSurfaceResizedEvent{ e.width, e.height });
		});

	pool.subscribe<FrameBufferResetEvent>([this, &pool](const FrameBufferResetEvent& e) {
		id_fb = static_cast<FrameBuffer*>(e.pass);
		viewport_fb = static_cast<FrameBuffer*>(e.result);

		const GLuint tex_id = viewport_fb->GetFBTextureID(COMBINE_FB);
		pool.emit(ViewportImageResetEvent{ tex_id });
		});
}
