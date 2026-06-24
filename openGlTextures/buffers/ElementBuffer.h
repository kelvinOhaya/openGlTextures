#pragma once
#include <glad/glad.h>
class ElementBuffer
{
public:
	unsigned int bufferId;
	ElementBuffer(const unsigned int* indices, size_t size);
	const void bind();
	 const void unbind();
};

