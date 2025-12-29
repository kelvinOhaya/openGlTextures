#pragma once

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <string>

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
	void setBool(std::string& name, bool value) const;
	//setting integer uniform variables
	void setInt(std::string& name, int value) const;
	//setting float uniform variables
	void setFloat(std::string& name, float value) const;

};

