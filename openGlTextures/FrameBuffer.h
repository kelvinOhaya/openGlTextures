#pragma once
#include <glad/glad.h>

class FrameBuffer
{
private: 
	unsigned int bufferId;
public:
	FrameBuffer();
	void bind();
	void unbind();

};

