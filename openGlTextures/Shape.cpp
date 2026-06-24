#include "Shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <ranges>
#include <iostream>
#include "shader.h"




glm::vec3 Shape::getNormal(glm::vec3 left, glm::vec3 center, glm::vec3 right)
{
    auto edgeOne = left - center;
    auto edgeTwo = right - center;

    auto result = glm::normalize(glm::cross(edgeOne, edgeTwo));

    return result;
}

void Shape::printModelMatrix()
{
    // Get a flat pointer to the 16 float elements
    const float* pSource = glm::value_ptr(modelMatrix);

    // GLM is column-major, so we loop over row and col carefully
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            // Indexing flat 1D array from 2D coordinates
            std::cout << pSource[col * 4 + row] << " ";
        }
        std::cout << "\n";
    }
}

Shape::Shape(float width, float height, float depth, const glm::vec3& pos, Camera& camera):camera(camera), color(glm::vec3(1,1,1)), mesh("models/cow.obj")
{
    glm::mat4 identity(1.0f);
    glm::mat4 translated(glm::translate(identity, pos));
    modelMatrix = glm::scale(translated, glm::vec3(width, height, depth));


    mesh.printStructure();
  
    addNormals();
    //turn shape into raw data
    flatten();
    bindBuffers();
    //print said data
    //printRawData();
        
}

void Shape::flatten(){

    //put the raw points into data
    for (int i = 0; i < mesh.vertices.size(); i++) {
        //push points into the row
        rawData.data.push_back(mesh.vertices[i].pos.x);
        rawData.data.push_back(mesh.vertices[i].pos.y);
        rawData.data.push_back(mesh.vertices[i].pos.z);

        //push normals into the row
        rawData.data.push_back(mesh.vertices[i].normal.x);
        rawData.data.push_back(mesh.vertices[i].normal.y);
        rawData.data.push_back(mesh.vertices[i].normal.z);
    }

}

const float* Shape::getRawData(){return rawData.data.data();}
unsigned int Shape::getRawSize(){return rawData.data.size() * sizeof(float);}
glm::mat4 Shape::getModelMatrix(){return modelMatrix;}

void Shape::printRawData()
{
    //print said data
    std::cout << "VERTICES AND NORMALS: \n";
    for (size_t i = 0; float d : rawData.data) {
        std::cout << ((d >= 0) ? " " : "") << d << ", ";
        if ((i + 1) % 6 == 0 && i > 0) {
            std::cout << "\n\n";
        }
        i++;
    }

    std::cout << "\n\nINDICES: \n";
    for (size_t i = 0; i < mesh.indices.size(); i+= 3) {
        std::cout << mesh.indices[i] << ", " << mesh.indices[i + 1] << ", " << mesh.indices[i + 2] << "\n";
    }
}

void Shape::addNormals()
{
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        glm::vec3 left, center, right;
        
        int v0 = mesh.indices[i];
        int v1 = mesh.indices[i + 1];
        int v2 = mesh.indices[i + 2];

        left = mesh.vertices[v0].pos;
        center = mesh.vertices[v1].pos;
        right = mesh.vertices[v2].pos;

        glm::vec3 result = getNormal(left, center, right);

        mesh.vertices[v0].normal = result;
        mesh.vertices[v1].normal = result;
        mesh.vertices[v2].normal = result;
    }
}

void Shape::draw()
{
    VBO->bind();
    VAO->bind();
    EBO->bind();
    glDrawElements(GL_LINES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void Shape::bindBuffers()
{
    VBO = std::make_unique<VertexBuffer>(getRawData(), getRawSize());
    VAO = std::make_unique<VertexAttribute>();
    EBO = std::make_unique<ElementBuffer>(mesh.indices.data(), mesh.indices.size() * sizeof(unsigned int));

    VAO->addPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    VAO->addPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
}

glm::vec3 Shape::getColor(){return color;}

void Shape::setColor(ShapeColor op) {
    switch (op) {
    case ShapeColor::RED:
        color = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case ShapeColor::GREEN:
        color = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case ShapeColor::BLUE:
        color = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case ShapeColor::YELLOW:
        color = glm::vec3(1.0f, 1.0f, 0.0f);
        break;
    case ShapeColor::CYAN:
        color = glm::vec3(0.0f, 1.0f, 1.0f);
        break;
    case ShapeColor::MAGENTA:
        color = glm::vec3(1.0f, 0.0f, 1.0f);
        break;
    case ShapeColor::WHITE:
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        break;
    case ShapeColor::BLACK:
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    }
}


Shape::Vertex::Vertex(const float* pos):coords(pos[0],pos[1],pos[2]),x(pos[0]),y(pos[1]),z(pos[2])
{ 
}

Shape::Vertex::Vertex(glm::vec3 pos):coords(pos.x, pos.y, pos.z),x(pos.x),y(pos.y),z(pos.z){}

Shape::Vertex::Vertex(float x, float y, float z):coords(x,y,z),x(x),y(y),z(z){}

void Shape::Vertex::flatten( float* target) {
   float* raw = glm::value_ptr(coords);
   target[0] = raw[0];
   target[1] = raw[1];
   target[2] = raw[2];
}





