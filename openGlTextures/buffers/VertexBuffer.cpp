#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const float*  vertices, unsigned int size)
{
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, size,vertices, GL_STATIC_DRAW);
}



VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &bufferId);
}

void VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
}

void VertexBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


