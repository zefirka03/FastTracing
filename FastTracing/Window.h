#pragma once
#include "globalGL.h"

class Window {
public:
	Window(int width, int height, std::string name, bool fs);
	GLFWwindow* getGLFWWindow();
	bool ShouldClose();
	void frameStart();
	void frameEnd();
	float get_deltaTime();
	~Window();

	GLFWwindow* win;
private:
	float deltaTime;
	float gpuInfo_timer;
	clock_t timer;
};

