#include "FBO.h"

FBO::FBO() {
	glGenFramebuffers(1, &fbo_id);
}

void FBO::initTexture(int x, int y) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FBO::getTexId() {
	return texColorBuffer;
}

void FBO::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

void FBO::clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void FBO::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::bindDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO() {
	glDeleteFramebuffers(1, &texColorBuffer);
	glDeleteFramebuffers(1, &fbo_id);
}