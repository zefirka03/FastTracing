#include "VAO.h"

VAO::VAO() {

}

void VAO::init() {

}

void VAO::vboInit() {
	glGenBuffers(1, &vbo_pos);
	glGenBuffers(1, &vbo_tex);
	glGenVertexArrays(1, &vao);
	shader = new Shader();
	this->bind();
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->unbind();
}

void VAO::vboRedata(GLsizeiptr size, const void* data, int count, data_type _type) {
	this->bind();

	if (_type == data_type::pos)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	else if (_type == data_type::tex)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);

	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->unbind();
	this->count = count;
}

void VAO::draw() {
	this->bind();
	shader->use();

	glDrawArrays(GL_TRIANGLES, 0, count);

	shader->unuse();
	this->unbind();
}

void VAO::setShader(Shader& sh) {
	this->shader = &sh;
}

void VAO::bind() {
	glBindVertexArray(this->vao);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

VAO::~VAO() {
	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_tex);
	glDeleteVertexArrays(1, &vao);
}