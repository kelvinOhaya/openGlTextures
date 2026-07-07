#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const float*  vertices, unsigned int size)
{
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBuffer::init(const float* vertices, unsigned int size) {
	if (bufferId == std::numeric_limits<unsigned int>::max()) {
		glGenBuffers(1, &bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
}

unsigned int VertexBuffer::getId() {
	return bufferId;
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

void VertexBuffer::updateData(const float* data, size_t size) {
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

