#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 view, model, projection;
uniform mat4 mvp;
uniform mat3 inverseNormal;

out vec2 texCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 heights;
out vec3 dis;

void main()
{
	//transformations are done from right to left
	gl_Position = mvp*vec4(aPos, 1.0f);
	vec4 viewport = vec4(aPos,1.0)*view;
	
	//assuming the opposites are 2 instances away from each other at all times
	//Utils::HEIGHTs are left, center, and right


	FragPos = vec3(model*vec4(aPos, 1.0f));
	//calculating the new normal after model transformation
	Normal = inverseNormal * aNormal;
}
