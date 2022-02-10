#include "FBO.h"

FBO::FBO() {
	glGenFramebuffers(1, &fbo_id);
}

void FBO::initTexture(int x, int y) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
	tex.bind();
	tex.init(x, y);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.getId(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FBO::getTexId() {
	return tex.getId();
}

void FBO::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}
void FBO::bindFBO_Texture() {
	tex.bind();
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
	glDeleteFramebuffers(1, &fbo_id);
}