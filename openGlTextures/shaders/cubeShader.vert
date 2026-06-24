#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 view, model, projection;

out vec2 texCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	//transformations are done from right to left
	gl_Position = projection*view*model*vec4(aPos, 1.0f);

	FragPos = vec3(model*vec4(aPos, 1.0f));
	//calculating the new normal after model transformation
	Normal = mat3(transpose(inverse(model))) * aNormal;
}
