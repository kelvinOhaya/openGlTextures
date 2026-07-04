#include "Texture2D.h"

Texture2D::Texture2D()
{
	glGenTextures(1, &textureId);
	bind();
}

void Texture2D::bind()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture2D::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::texImage(GLenum target,
    GLint level,
    GLint internalformat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLenum format,
    GLenum type,
    const void* data)
{
    glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}

void Texture2D::parameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void Texture2D::minLinearParam()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture2D::magLinearParam()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::minNearestParam()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture2D::magNearestParam()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

int Texture2D::getID() { return textureId; }

void Texture2D::deleteSelf() {
    glDeleteTextures(1, &textureId);
}
void Texture2D::resize(int width, int height) {
    bind();
    texImage(GL_TEXTURE_2D, 0, GL_RGB32UI, width, height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, nullptr);
    unbind();
}

Texture2D::~Texture2D() {
    deleteSelf();
}