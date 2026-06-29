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
#include "Shape.h"
#include <ranges>
#include "shader.h"

enum class ShapeColor {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN,
	MAGENTA,
	WHITE,
	BLACK
};

class Shape
{
private:

	glm::vec3 color;

	//member functions
    glm::vec3 getNormal(glm::vec3 left, glm::vec3 center, glm::vec3 right);
	
	//debug functions
	void printModelMatrix();

	//classes
	class RawData {
	public:
		std::vector<float> data;
		int size;
	};
	class Vertex {
	public:
		float x;
		float y;
		float z;
	
		glm::vec3 coords;
		glm::vec3 normal;

		//flatten the vertex into a raw array
		void flatten();
		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertexObj) {
			return os << "Coordinates: " << "( x:" << vertexObj.coords.x << ", y:" << vertexObj.coords.y << ", z:" << vertexObj.coords.z << " )" << std::endl;
		}
		
	};

	

	
public:
	
	RawData rawData;
	//constructor
	Shape() = delete;
	Shape(std::string filename, float width, float height, float depth, const glm::vec3& pos, Camera& camera);

	void flatten();

	//member variables
	std::unique_ptr<VertexBuffer> VBO;
	std::unique_ptr<VertexAttribute> VAO;
	std::unique_ptr<ElementBuffer> EBO;
	std::vector<float> vertices;

	//model vectors
	glm::vec3 rotation;
	glm::vec3 translation;
	glm::vec3 scale;

	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 scaleMatrix;
	unsigned int bufferSize;

	//rotation values
	float rotateX = 0.0f; 
	float rotateY = 0.0f; 
	float rotateZ = 0.0f;
	
	//translation values
	float translateX = 1.0f; 
	float translateY = 1.0f; 
	float translateZ = 1.0f;
	glm::mat4 modelMatrix;
	Mesh mesh;
	const Camera& camera;


	//methods
	void updateModelMatrix();
	const float* getRawData();
	unsigned int getRawSize();
	void printRawData();
	void addNormals();
	void draw();
	void bindBuffers();
	glm::vec3 getColor();
	void setColor(ShapeColor op);

};

