#include "Shader.h"

Shader::Shader() {

}

void Shader::Init(std::string path, bool lock) {
	locker = lock;
	this->prog_id = glCreateProgram();
	std::ifstream of;
	of.open(path.c_str());
	std::string curr;
	int type = 0;
	std::string vertex_source = "",
		fragment_source = "";
	while (std::getline(of, curr)) {
		std::string cur2 = "";
		if (locker) {
			for (int i = 0; i < curr.size(); i += 2)
				cur2 += curr[i];
		}
		else cur2 = curr;
		if (cur2 == "~~vertex~~") type = 0;
		else if (cur2 == "~~fragment~~") type = 2;
		else {
			if (type == 0) vertex_source += cur2 + "\n";
			else if (type == 2) fragment_source += cur2 + "\n";
		}
	}

	const char* vertex_source_c = vertex_source.c_str(),
		* fragment_source_c = fragment_source.c_str();

	GLuint	vertex_id = glCreateShader(GL_VERTEX_SHADER),
		geomerty_id = glCreateShader(GL_GEOMETRY_SHADER),
		fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex_id, 1, &vertex_source_c, 0);
	glShaderSource(fragment_id, 1, &fragment_source_c, 0);

	glCompileShader(geomerty_id);
	glCompileShader(vertex_id);
	glCompileShader(fragment_id);
	GLchar info[2048];
	glGetShaderInfoLog(vertex_id, 2048, 0, info);
	std::cout << info;
	glGetShaderInfoLog(fragment_id, 2048, 0, info);
	std::cout << info;

	glAttachShader(this->prog_id, vertex_id);
	glAttachShader(this->prog_id, fragment_id);

	glLinkProgram(this->prog_id);

	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
}

void Shader::use() {
	glUseProgram(prog_id);
}

void Shader::unuse() {
	glUseProgram(0);
}

void Shader::setUniformV3(std::string name, glm::vec3 data) {
	use();
	glUniform3f(glGetUniformLocation(this->prog_id, name.c_str()), data.x, data.y, data.z);
	unuse();
}

void Shader::setUniformV2(std::string name, glm::vec2 data) {
	use();
	glUniform2f(glGetUniformLocation(this->prog_id, name.c_str()), data.x, data.y);
	unuse();
}

void Shader::setUniform1f(std::string name, float data) {
	use();
	glUniform1f(glGetUniformLocation(this->prog_id, name.c_str()), data);
	unuse();
}

void Shader::setUniform1i(std::string name, int data) {
	use();
	glUniform1i(glGetUniformLocation(this->prog_id, name.c_str()), data);
	unuse();
}

void Shader::setCamera(Camera camera, const char* name) {
	this->setUniformV3(std::string(name) + ".cameraPos", camera.cameraPos);
	this->setUniformV3(std::string(name) + ".cameraUp", camera.cameraUp);
	this->setUniformV3(std::string(name) + ".cameraRight", camera.cameraRight);
	this->setUniformV3(std::string(name) + ".cameraFront", camera.cameraFront);
	this->setUniform1f(std::string(name) + ".fov", camera.fov);
	this->setUniform1f(std::string(name) + ".dist", camera.dist);
}


Shader::~Shader() {
	glDeleteProgram(this->prog_id);
}