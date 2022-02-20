#pragma once
#include "globalGL.h"
#include "Camera.h"

class Shader {
public:
	Shader();
	void Init(std::string path, bool locker);
	void use();
	void unuse();
	void setUniformV3(std::string name, glm::vec3 data);
	void setUniformV2(std::string name, glm::vec2 data);
	void setUniform1f(std::string name, float data);
	void setUniform1i(std::string name, int data);
	void setCamera(Camera camera, const char* name);
	~Shader();
	GLuint prog_id;
	bool locker = true;
};

