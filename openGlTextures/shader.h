#pragma once

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	//the ID of our shader program
	unsigned int ID;

	//our constructor
	Shader(const char* vertexPath, const char* fragmentPath);

	//wrapper for using the shader program
	void use();

	//seting boolean uniform variables
	void setBool(const char* name, bool value) const;
	//setting integer uniform variables
	void setInt(const char* name, int value) const;
	//setting float uniform variables
	void setFloat(const char* name, float value) const;
	//setting 4x4 matrix uniform variables
	void setMatrix4f(const char* name, glm::mat4 value) const;
	void setMatrix3f(const char* name, glm::mat3 value) const;
	void setVec3(const char* name, glm::vec3 value) const;

};

