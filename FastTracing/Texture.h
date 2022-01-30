#pragma once
#include "globalGL.h"

class Texture{
public:
	Texture();
	void loadFromFile(const char* path);
	void bind();
	static void unbind();
	~Texture();
private:
	GLuint tex_id;
	int wh, hh;
};

