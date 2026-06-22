#pragma once
#include <GLAD/glad.h>

class VertexBuffer
{
private:
	unsigned int bufferId;
public:
	VertexBuffer(const float* vertices, unsigned int size);
	~VertexBuffer();
	void bind();
	void unbind();
};

