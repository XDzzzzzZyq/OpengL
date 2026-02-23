/**
 * @file ViewportController.h
 * @brief Viewport interaction controller for object picking and viewport management.
 * 
 * ViewportController handles user interactions within the 3D viewport, including
 * object selection via mouse picking, viewport resize events, and coordinate
 * transformations between screen space and world space.
 * 
 * Architecture:
 * - Subscribes to viewport-specific events (click, resize, focus)
 * - Maintains framebuffer references for picking and display
 * - Performs ray-casting from screen coordinates to 3D world
 * - Emits SelectionEvents when objects are picked
 * 
 * Picking Pipeline:
 * 1. Mouse click in viewport → screen coordinates (x, y)
 * 2. Read id_fb at (x, y) to get object ID (GPU-based picking)
 * 3. Emit SelectionEvent with picked ObjectID
 * 4. SelectionManager updates, UI highlights selected object
 * 
 * Framebuffer Roles:
 * - id_fb: Offscreen buffer rendering object IDs as colors (picking)
 * - viewport_fb: Main render target displayed in UI viewport panel
 * 
 * Coordinate Systems:
 * - Screen: (0,0) at top-left, Y-down (ImGui convention)
 * - NDC: (-1,-1) to (1,1), Y-up (OpenGL convention)
 * - World: Right-handed Y-up global space
 * 
 * @note ViewportController does not own framebuffers - pointers are non-owning.
 * @note Picking requires id_fb to be rendered with unique ID colors per object.
 */

#pragma once

#include "Controllers.h"
#include "buffers/FrameBuffer.h"

/**
 * @brief Controller for viewport interaction and object selection.
 * 
 * ViewportController manages user interactions within the 3D viewport, primarily
 * object selection via GPU-based picking. It maintains references to framebuffers
 * used for picking (ID buffer) and display (viewport render target).
 * 
 * Selection Mechanism:
 * - GPU renders scene with object IDs encoded as colors to id_fb
 * - Mouse click reads pixel from id_fb at cursor position
 * - Color decoded back to ObjectID
 * - SelectionEvent emitted with picked object
 * 
 * Viewport Management:
 * - Handles ViewportResizeEvent to update framebuffer dimensions
 * - Maintains aspect ratio for camera projection updates
 * - Coordinates with Renderer for framebuffer recreation
 * 
 * Event Integration:
 * - Subscribes to: MouseClickEvent, ViewportResizeEvent, ViewportFocusEvent
 * - Emits: SelectionEvent (object picked), ViewportResizeEvent (size changed)
 * 
 * State Management:
 * - Reads viewport dimensions from UI layer
 * - Queries active camera from SceneContext for ray generation
 * - Updates SelectionManager via emitted events
 * 
 * @note Framebuffer pointers are non-owning - managed by Renderer.
 * @note Picking requires id_fb to be rendered before interaction.
 */
class ViewportController : public Controllers
{
private:
	FrameBuffer* id_fb{ nullptr };       ///< Non-owning pointer to ID framebuffer for picking
	FrameBuffer* viewport_fb{ nullptr }; ///< Non-owning pointer to main viewport framebuffer
public:
	/**
	 * @brief Bind to EventPool to subscribe to viewport interaction events.
	 * 
	 * Subscribes to events for viewport interactions:
	 * - MouseClickEvent: Perform object picking via id_fb read
	 * - MouseMoveEvent: Handle hover feedback (optional)
	 * - ViewportResizeEvent: Update framebuffer dimensions
	 * - ViewportFocusEvent: Handle viewport activation/deactivation
	 * 
	 * Picking Logic:
	 * 1. On click, query mouse position in viewport coordinates
	 * 2. Read pixel from id_fb at (x, y)
	 * 3. Decode RGB color to ObjectID
	 * 4. Emit SelectionEvent if valid object picked
	 * 
	 * Resize Logic:
	 * 1. Receive ViewportResizeEvent with new dimensions
	 * 2. Notify Renderer to recreate framebuffers
	 * 3. Update camera aspect ratio via CameraEvent
	 * 
	 * @param pool EventPool to subscribe to for viewport events
	 * 
	 * @note Framebuffer pointers must be set before handling click events.
	 * @note ID buffer rendering must occur before picking is valid.
	 */
	virtual void bind(EventPool& pool) override;
};

