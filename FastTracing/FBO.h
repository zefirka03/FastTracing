#pragma once
#include "globalGL.h"

class FBO {
public:
	FBO();
	void initTexture(int x, int y);
	GLuint getTexId();
	void bind();
	void clear();
	void unbind();
	static void bindDefault();
	~FBO();
private:
	GLuint fbo_id;
	GLuint texColorBuffer;
};

