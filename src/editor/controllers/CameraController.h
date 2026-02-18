/**
 * @file CameraController.h
 * @brief Camera manipulation controller for interactive viewpoint control.
 * 
 * CameraController handles user input events (mouse drag, scroll, keyboard) and
 * translates them into camera transform updates. It implements standard 3D viewport
 * navigation patterns: orbit, pan, zoom, and fly-through movement.
 * 
 * Architecture:
 * - Subscribes to mouse and keyboard events via EventPool
 * - Modifies active Camera object's Transform3D directly
 * - Emits CameraEvents to notify other systems of camera changes
 * - Operates in world space or camera-local space based on mode
 * 
 * Camera Modes:
 * - Orbit: Rotate around look-at target (tumble)
 * - Pan: Translate camera parallel to view plane
 * - Zoom: Move camera toward/away from target (dolly)
 * - Fly: Free-form movement without target constraint
 * 
 * Coordinate System:
 * - Right-handed Y-up world space
 * - Camera forward: -Z axis (OpenGL convention)
 * - Transform3D provides position/rotation as glm::vec3
 * 
 * @note Mouse sensitivity and movement speed are configurable.
 * @note Camera bounds (near/far planes) are enforced during manipulation.
 */

#pragma once

#include "Controllers.h"

/**
 * @brief Controller for interactive camera manipulation via user input.
 * 
 * CameraController translates mouse and keyboard input into camera transform
 * updates, enabling standard 3D viewport navigation. It subscribes to input
 * events and directly modifies the active Camera's position and rotation.
 * 
 * Supported Operations:
 * - **Orbit (Alt+LMB Drag)**: Rotate camera around target point
 * - **Pan (Alt+MMB Drag)**: Translate camera perpendicular to view direction
 * - **Zoom (Mouse Wheel)**: Move camera toward/away from target
 * - **Fly (Shift+WASD)**: Free-form movement without target constraint
 * 
 * Event Integration:
 * - Subscribes to: MouseButtonEvent, MouseMoveEvent, MouseScrollEvent, KeyEvent
 * - Emits: CameraEvent (on transform change), CameraTargetEvent (on target update)
 * 
 * State Management:
 * - Reads active camera from SceneContext
 * - Updates camera Transform3D (position, rotation)
 * - Maintains look-at target for orbit mode (stored in Camera::cam_tar)
 * 
 * Mathematical Conventions:
 * - Orbit uses spherical coordinates (azimuth, elevation, radius)
 * - Pan uses camera's right and up vectors for screen-space movement
 * - Zoom modifies distance from target while preserving direction
 * 
 * @note CameraController does not own the camera - it operates on active camera from Context.
 * @note Multiple cameras can exist; controller affects whichever is currently active.
 */
class CameraController : public Controllers
{
public:
	/**
	 * @brief Bind to EventPool to subscribe to input events.
	 * 
	 * Subscribes to mouse and keyboard events required for camera manipulation:
	 * - MouseButtonEvent: Detect drag start/end
	 * - MouseMoveEvent: Track cursor delta for orbit/pan
	 * - MouseScrollEvent: Handle zoom via wheel
	 * - KeyEvent: WASD for fly mode, modifiers for mode switching
	 * 
	 * Event handlers query SceneContext for active camera and update its transform
	 * based on input deltas. Changes are applied incrementally each frame.
	 * 
	 * @param pool EventPool to subscribe to for input events
	 * 
	 * @note Handlers are stateless - camera state lives in Camera object.
	 * @note Input sensitivity can be configured via constants or settings.
	 */
	virtual void bind(EventPool& pool) override;
};

