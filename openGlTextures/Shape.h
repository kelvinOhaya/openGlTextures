#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "buffers/VertexAttribute.h"
#include "buffers/VertexBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/detail/setup.hpp>
#include <glm/gtc/type_ptr.hpp> 


#include <iostream>
#include <array>
#include "camera.h"
#include <optional>
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
	//member variables
	std::unique_ptr<VertexBuffer> VBO;
	std::unique_ptr<VertexAttribute> VAO;
	glm::mat4 modelMatrix;
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
		Vertex(const float* pos);
		Vertex(glm::vec3 pos);
		Vertex(float x, float y, float z);
		//flatten the vertex into a raw array
		void flatten( float* target);
		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertexObj) {
			return os << "Coordinates: " << "( x:" << vertexObj.coords.x << ", y:" << vertexObj.coords.y << ", z:" << vertexObj.coords.z << " )" << std::endl;
		}
		
	};

	

	
public:
	RawData rawData;
	//constructor
	Shape() = delete;
	Shape(float width, float height, float depth, const glm::vec3& pos, Camera& camera);

	void flatten(RawData& rawData);

	//member variables
	std::vector<float> vertices;
	unsigned int bufferSize;
	std::vector<Vertex> points;
	const Camera& camera;


	//methods
	glm::mat4 getModelMatrix();
	const float* getRawData();
	unsigned int getRawSize();
	void printRawData();
	void addNormals();
	void draw();
	void bindBuffers();
	glm::vec3 getColor();
	void setColor(ShapeColor op);
};

