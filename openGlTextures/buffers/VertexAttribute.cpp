#include "VertexAttribute.h"

VertexAttribute::VertexAttribute()
{
	glGenVertexArrays(1, &vertexArrayId);
	bind();
}

VertexAttribute::~VertexAttribute()
{
	glDeleteVertexArrays(1, &vertexArrayId);
}

void VertexAttribute::bind()
{
	glBindVertexArray(vertexArrayId);
}

void VertexAttribute::unbind()
{
	glBindVertexArray(0);
}

void VertexAttribute::enable(GLuint index)
{
	glEnableVertexAttribArray(index);
}

void VertexAttribute::disable(GLuint index)
{
	glDisableVertexAttribArray(index);
}

void VertexAttribute::addPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	enable(index);
}


