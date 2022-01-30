#pragma once
#include "globalGL.h"
#include "Window.h"

class Camera {
public:
	Camera();
	bool update(Window& win);
	~Camera();
	glm::vec3	cameraPos,
		cameraFront,
		cameraUp,
		cameraRight;
	float fov, dist;
	float speed, sense;
private:
	float pitch, yaw;
	glm::vec2 last_mouse;
};

