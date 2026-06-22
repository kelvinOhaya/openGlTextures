#pragma once
#include <glad/glad.h>
class ElementBuffer
{
public:
	unsigned int bufferId;
	ElementBuffer(const float* indices, unsigned int size);
	 const void bind();
	 const void unbind();
};

