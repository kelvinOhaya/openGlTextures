#version 330 core

uniform uint objIdx;
out uvec4 id;


void main()
{
	id = uvec4(objIdx, 0,0,0);
}
