#include "Camera.h"
#define PI 3.1415926535
Camera::Camera() {
	cameraPos = glm::vec3(0);
	//scene1
	//cameraPos = glm::vec3(386.482f, 97.0795f, 41.2703f);
	//cameraFront = glm::vec3(-0.631185f, -0.0169991, 0.775446f);

	//scene2
	//cameraPos = glm::vec3(976.261f, 180.367f, 40.021f);
	//cameraFront = glm::vec3(-0.6729f, -0.3588f, 0.6468f);

	cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0)));
	cameraUp = glm::normalize(glm::cross(cameraFront, -cameraRight));

	//cameraUp = glm::vec3(0, 1, 0);
	//cameraRight = glm::vec3(1, 0, 0);
	fov = 2 * PI / 3;
	dist = 0.5f;
	speed = 0.08f;
	sense = 0.001f;
	last_mouse = glm::vec2(0);
	pitch = 0;
	yaw = 0;
}

bool Camera::update(Window& win) {
	bool fl = false;
	if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_LEFT_SHIFT)) speed = 150.f;
	else if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_LEFT_CONTROL)) speed = 5.f;
	else speed = 15.f;

	if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_W)) {
		cameraPos += cameraFront * speed * win.get_deltaTime();
		fl = true;
	}
	if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_S)) {
		cameraPos += -cameraFront * speed * win.get_deltaTime();
		fl = true;
	}
	if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_D)) {
		cameraPos += cameraRight * speed * win.get_deltaTime();
		fl = true;
	}
	if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_A)) {
		cameraPos += -cameraRight * speed * win.get_deltaTime();
		fl = true;
	}



	double x, y;
	glfwGetCursorPos(win.getGLFWWindow(), &x, &y);
	glm::vec2 delta(x - last_mouse.x,
		last_mouse.y - y);
	last_mouse = glm::vec2(x, y);

	if (delta.x || delta.y) fl = true;

	yaw += delta.x * sense;
	pitch += delta.y * sense;
	if (pitch > PI / 2 - 0.1f) pitch = PI / 2 - 0.1f;
	if (pitch < -PI / 2 + 0.1f) pitch = -PI / 2 + 0.1f;

	cameraFront = glm::normalize(glm::vec3(cos(yaw) * cos(pitch),
		sin(pitch),
		sin(yaw) * cos(pitch)));
	cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0)));
	cameraUp = glm::normalize(glm::cross(cameraFront, -cameraRight));

	//std::cout << delta.x << " " << delta.y<<'\n';
	return fl;
}

Camera::~Camera() {

}