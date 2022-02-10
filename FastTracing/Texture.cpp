#include "Texture.h"

Texture::Texture(){
	glGenTextures(1, &tex_id); 
	glActiveTexture(GL_TEXTURE0);
}

void Texture::loadFromFile(const char* path) {
	unsigned char* image = SOIL_load_image(path, &wh, &hh, 0, SOIL_LOAD_RGB);
	this->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wh, hh, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	this->unbind();
}

void Texture::init(int x, int y) {
	wh = x, hh = y;
	this->bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wh, hh, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	this->unbind();
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, tex_id);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getId() {
	return tex_id;
}


Texture::~Texture(){}