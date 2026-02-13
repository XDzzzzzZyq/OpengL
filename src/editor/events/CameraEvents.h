#include "Camera.h"

struct CameraZoomEvent {
	Camera* cam;
	float scroll_dir;
};

struct CameraRotateEvent {
	Camera* cam;
	float mouse_delta_x, mouse_delta_y;
};

struct CameraPushEvent {
	Camera* cam;
	float mouse_delta_x, mouse_delta_y;
};

struct CameraSlideEvent {
	Camera* cam;
	float mouse_delta_x, mouse_delta_y;
};

struct CameraSpinEvent {
	Camera* cam;
	float mouse_delta_x, mouse_delta_y;
};