#pragma once
#include <GLAD/glad.h>
#include <limits>

class VertexBuffer
{
private:
	unsigned int bufferId;
public:
	VertexBuffer() : bufferId(std::numeric_limits<unsigned int>::max()) {};
	VertexBuffer(const float* vertices, unsigned int size);
	void init(const float* vertices, unsigned int size);
	unsigned int getId();
	~VertexBuffer();
	void bind();
	void unbind();
	void updateData(GLintptr offset, const float* data, size_t size);
};

