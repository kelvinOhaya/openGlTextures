#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 clipped;

out vec3 ourColor;

void main()
{
	//transformations are done from right to left
	gl_Position = clipped*vec4(aPos, 1.0f);
}
