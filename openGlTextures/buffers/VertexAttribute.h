#pragma once
#include <glad/glad.h>
class VertexAttribute
{
public:	
	VertexAttribute();
	~VertexAttribute();
	void bind();
	void unbind();
	void enable(GLuint index);
	void disable(GLuint index);
	void addPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,  const GLvoid *pointer);

private:
	unsigned int vertexArrayId;
};

