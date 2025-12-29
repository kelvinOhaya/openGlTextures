#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath):ID(0) {

	//string versions of our code. Will be used to turn into a c_string
	std::string vertexCode;
	std::string fragmentCode;

	//file reading objects
	std::ifstream vertexFile;
	std::ifstream fragmentFile;

	//exception handling
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	//open the file and read it into the strings provided above
	try
	{
		//our stringstreams that hold the raw file data
		std::stringstream vertexStringStream, fragmentStringStream;

		//open the files
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		
		//put the file buffer into the stringStream
		vertexStringStream << vertexFile.rdbuf();
		fragmentStringStream << fragmentFile.rdbuf();

		//close the files
		vertexFile.close();
		fragmentFile.close();

		//put the stringStreams into the string objects defined at the start
		vertexCode = vertexStringStream.str();
		fragmentCode = fragmentStringStream.str();
	}
	catch (const std::exception& e)
	{
		std::cout << "Error reading the files: \n" << e.what() << std::endl;
	}

	//store the code in a c_string for openGL
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//shader comilation process
	unsigned int vertex, fragment, shaderProgram;
	int success;
	char infoLog[512];

	//shader creation
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	//shader source code being added
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glShaderSource(fragment, 1, &fShaderCode, NULL);

	//compile the shaders
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "VERTEX::SHADER::COMPILATION::ERROR: " << infoLog << std::endl;
	}

	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "FRAGMENT::SHADER::COMPILATION::ERROR: " << infoLog << std::endl;
	}

	//create the shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);

	//link shaders together
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR:PROGRAM:LINKING:ERROR:" << infoLog << std::endl;
	}

	//delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

//use the program (js a wrapper)
void Shader::use() {
	glUseProgram(ID);
}

//helper functions for changing uniform variables
void Shader::setBool(std::string& name, bool value) const 
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
