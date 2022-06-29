#include "Window.h"

Window::Window(int width, int height, std::string name, bool fs) {
	glfwInit();
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	if (fs) this->win = glfwCreateWindow(width, height, name.c_str(), glfwGetPrimaryMonitor(), NULL);
	else this->win = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	glfwMakeContextCurrent(this->win);

	glewInit();
	glClearColor(0.f, 0.f, 0.f, 0.f);
	deltaTime = 0;
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_BLEND);

	//glfwSwapInterval(0);

	gpuInfo_timer = 10000;
}

GLFWwindow* Window::getGLFWWindow() {
	return win;
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(win) || glfwGetKey(this->getGLFWWindow(), GLFW_KEY_ESCAPE);
}

void Window::frameStart() {
	timer = clock();
	glClear(GL_COLOR_BUFFER_BIT);
	
	GLint cur_avail_mem_kb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
		&cur_avail_mem_kb);
	if (gpuInfo_timer > 2) {
		gpuInfo_timer = 0;
		printf("AVAILABLE %d MB\n", cur_avail_mem_kb / 1024);
	}
}
void Window::frameEnd() {
	glfwSwapBuffers(this->win);
	glfwPollEvents();
	deltaTime = (float)(clock() - timer) / (float)CLOCKS_PER_SEC;

	gpuInfo_timer += deltaTime;
}

float Window::get_deltaTime() {
	return deltaTime;
}

Window::~Window() {
	glfwTerminate();
}