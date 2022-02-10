#pragma once
#include "globalGL.h"

class Texture{
public:
	Texture();
	void loadFromFile(const char* path);
	void init(int x, int y);
	void bind();
	GLuint getId();
	static void unbind();
	~Texture();
private:
	GLuint tex_id;
	int wh, hh;
};

