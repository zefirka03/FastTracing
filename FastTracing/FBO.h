#pragma once
#include "globalGL.h"
#include "Texture.h"

class FBO {
public:
	FBO();
	void initTexture(int x, int y);
	GLuint getTexId();
	void bind();
	void clear();
	void unbind();
	void bindFBO_Texture();
	static void bindDefault();
	~FBO();
private:
	GLuint fbo_id;
	Texture tex;
};

