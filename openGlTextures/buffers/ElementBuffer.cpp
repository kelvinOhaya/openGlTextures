#include "ElementBuffer.h"
#include <limits>

ElementBuffer::ElementBuffer(const unsigned int* indices,size_t size):bufferId(std::numeric_limits<unsigned int>::max())
{

	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices,GL_STATIC_DRAW);
}

const void ElementBuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
}

const void ElementBuffer::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
