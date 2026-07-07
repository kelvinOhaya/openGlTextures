#include "Shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <ranges>
#include <iostream>
#include "shader.h"


Shape::Shape(std::string filename, float width, float height, float depth, const glm::vec3& pos, Camera& camera):camera(camera), color(glm::vec3(1,1,1)), mesh(filename), translation(pos), scale(glm::vec3(width, height, depth))
{

    updateModelMatrix();
    //mesh.printStructure();
    flatten();
    bindBuffers();
    //print said data
    mesh.printTreeMetrics();
    //printRawData();
        
}

void Shape::flatten(){
    rawData.data.clear();
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

void Shape::scaleMesh(float width, float height, float depth)
{
    for (auto& v : mesh.vertices) {
        v.pos.x *= width;
        v.pos.y *= height;
        v.pos.z *= depth;
    }
    flatten();
}

void Shape::updateModelMatrix() {
    glm::mat4 identity(1.0f);
    translationMatrix = glm::translate(identity, translation);
    rotationMatrix = glm::rotate(identity, glm::radians(rotateX), glm::vec3(1, 0, 0))* glm::rotate(identity, glm::radians(rotateY), glm::vec3(0, 1, 0))* glm::rotate(identity, glm::radians(rotateZ), glm::vec3(0, 0, 1));
    scaleMatrix = glm::scale(identity, scale);
    modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
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

        mesh.vertices[v0].normal += result;
        mesh.vertices[v1].normal += result;
        mesh.vertices[v2].normal += result;
    }
    for (auto& vert : mesh.vertices) {
        if (glm::length(vert.normal) > 0.0f) {
            vert.normal = glm::normalize(vert.normal);
        }
    }
}

void Shape::draw()
{
    VAO->bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    VAO->unbind();

}

void Shape::bindBuffers()
{
    VAO = std::make_unique<VertexAttribute>();
    VBO = std::make_unique<VertexBuffer>(getRawData(), getRawSize());
    EBO = std::make_unique<ElementBuffer>(mesh.indices.data(), mesh.indices.size() * sizeof(unsigned int));
    //for reading vertices
    VAO->addPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //for reading normals
    VAO->addPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));

    // Explicitly attach the index buffer inside the open VAO scope
    EBO->bind();

    // Seal the record safely to isolate Yoshi's state
    VAO->unbind();
}
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








