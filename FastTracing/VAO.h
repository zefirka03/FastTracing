#pragma once
#include "globalGL.h"
#include "Shader.h"

enum data_type {
	tex,
	pos
};

class VAO {
public:
	VAO();
	void bind();
	void init();
	void unbind();
	void vboRedata(GLsizeiptr size, const void* data, int count, data_type _type);
	void vboInit();
	void setShader(Shader& sh);
	void draw();
	~VAO();
	GLuint vbo_pos, vbo_tex, vao;
	Shader* shader;
	int count;
};