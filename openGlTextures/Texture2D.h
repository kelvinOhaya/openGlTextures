#pragma once
#include <glad/glad.h>
class Texture2D
{
private:
	unsigned int textureId;
public:
	Texture2D();
    ~Texture2D();
	void bind();
	void unbind();
	void texImage(GLenum target,
        GLint level,
        GLint internalformat,
        GLsizei width,
        GLsizei height,
        GLint border,
        GLenum format,
        GLenum type,
        const void* data);
    void parameteri(GLenum target, GLenum pname, GLint param);
    //shorthand for setting minimum filtering to linear
    void minLinearParam();
    //shorthand for setting magifying filtering to linear
    void magLinearParam();
    //shorthand for setting minimum filtering to nearest neighbor
    void minNearestParam();
    //shorthand for setting magnifying filtering nearest neighbor
    void magNearestParam();
    //returns the underlying ID
    int getID();
    void deleteSelf();
    void resize(int width, int height);
};

