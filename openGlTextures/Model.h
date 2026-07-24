#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "buffers/VertexAttribute.h"
#include "buffers/VertexBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glad/glad.h> 
#include <iostream>
#include <array>
#include "camera.h"
#include <optional>
#include "buffers/ElementBuffer.h"
#include "Mesh.h"
#include <ranges>
#include "shader.h"

enum class ModelColor {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN,
	MAGENTA,
	WHITE,
	BLACK
};

class Model
{
private:
	glm::vec3 color = glm::vec3(0,0,0);
	bool mIsInitialized = false;
	//debug functions
	void printModelMatrix();
	//classes
	class RawData {
	public:
		std::vector<float> data;
		int size=0;

		RawData() {};
	
	};
		
public:
	bool isInitialized();
	//constructor
	RawData rawData;
	void init(std::string filename);
	 
	Model() = default;
	void flatten();

	//member variables
	std::unique_ptr<VertexBuffer> VBO = nullptr;
	std::unique_ptr<VertexAttribute> VAO = nullptr;
	std::unique_ptr<ElementBuffer> EBO = nullptr;
	std::vector<float> vertices;

	//model vectors
	glm::vec3 rotation = glm::vec3(1.0f);
	glm::vec3 translation = glm::vec3(1.0f);
	float scaleFactor = 1;

	glm::mat4 translationMatrix = glm::mat4(1);
	glm::mat4 rotationMatrix = glm::mat4(1);
	glm::mat4 scaleMatrix = glm::mat4(1);
	unsigned int bufferSize=0;

	//rotation values
	float rotateX = 0.0f; 
	float rotateY = 0.0f; 
	float rotateZ = 0.0f;
	
	//translation values
	float translateX = 1.0f; 
	float translateY = 1.0f; 
	float translateZ = 1.0f;
	glm::mat4 modelMatrix= glm::mat4(1.0f);

	//other
	std::unique_ptr<Mesh> mesh = nullptr;
	bool hasBeenClicked = false;


	//methods
	void scaleMesh(float width, float height, float depth);
	void checkIfInitialized();
	void updateModelMatrix();
	const float* getRawData();
	unsigned int getRawSize();
	void printRawData();
	void draw();
	void bindBuffers();
	glm::vec3 getColor();
	void setColor(ModelColor op);

};

