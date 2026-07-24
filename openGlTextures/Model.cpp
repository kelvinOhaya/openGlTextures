#include "Model.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <ranges>
#include <iostream>
#include "shader.h"





void Model::init(std::string filename)
{
    setColor(ModelColor::GREEN);
    mesh = std::make_unique<Mesh>(filename);
    mIsInitialized = true;
    updateModelMatrix();
    //mesh->printStructure();
    flatten();
    bindBuffers();
    mIsInitialized = true;
    //print said data
    //printRawData();
        
}
void Model::checkIfInitialized() {
    if (isInitialized()) {
        return;
    }
    else {
        std::cerr << "Initialize shape first";
        std::exit(1);
    }
}
bool Model::isInitialized() { return mIsInitialized; }
void Model::flatten() {
    checkIfInitialized();
    rawData.data.clear();
    //put the raw points into data
    for (int i = 0; i < mesh->vHandles.size(); i++) {

        myMesh::Point pos = mesh->data.point(mesh->vHandles[i]);
        myMesh::Normal norm = mesh->data.normal(mesh->vHandles[i]);
        //push position coordinates
        rawData.data.push_back(pos[0]);
        rawData.data.push_back(pos[1]);
        rawData.data.push_back(pos[2]);

        //push normals into the row
        rawData.data.push_back(norm[0]);
        rawData.data.push_back(norm[1]);
        rawData.data.push_back(norm[2]);
    }
    //mesh->printVertices();
    //mesh->printIndices();
}

const float* Model::getRawData(){
    checkIfInitialized();
    return rawData.data.data();}
unsigned int Model::getRawSize(){
    checkIfInitialized();
    return rawData.data.size() * sizeof(float);}



void Model::printRawData()
{
    checkIfInitialized();

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
    for (size_t i = 0; i < mesh->indices.size(); i+= 3) {
        std::cout << mesh->indices[i] << ", " << mesh->indices[i + 1] << ", " << mesh->indices[i + 2] << "\n";
    }
}

void Model::scaleMesh(float width, float height, float depth)
{
    checkIfInitialized();
    for (auto& vh : mesh->vHandles) {
        myMesh::Point currentPoint = mesh->data.point(vh);
        mesh->data.set_point(vh, myMesh::Point(currentPoint[0] * width, currentPoint[1] * height, currentPoint[2] * depth));
    }
    flatten();
    VBO->updateData(0, rawData.data.data(), rawData.data.size() * sizeof(float));
}

void Model::updateModelMatrix() {
    checkIfInitialized();

    glm::mat4 identity(1.0f);
    translationMatrix = glm::translate(identity, translation);
    rotationMatrix = glm::rotate(identity, glm::radians(rotateX), glm::vec3(1, 0, 0))* glm::rotate(identity, glm::radians(rotateY), glm::vec3(0, 1, 0))* glm::rotate(identity, glm::radians(rotateZ), glm::vec3(0, 0, 1));
    scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
    modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}



void Model::draw()
{
    checkIfInitialized();

    VAO->bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    VAO->unbind();

}

void Model::bindBuffers()
{
    VAO = std::make_unique<VertexAttribute>();
    VBO = std::make_unique<VertexBuffer>(getRawData(), getRawSize());
    EBO = std::make_unique<ElementBuffer>(mesh->indices.data(), mesh->indices.size() * sizeof(unsigned int));
    //for reading vertices
    VAO->addPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //for reading normals
    VAO->addPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));

    // Explicitly attach the index buffer inside the open VAO scope
    EBO->bind();
    // Seal the record safely to isolate Yoshi's state
    VAO->unbind();
}
void Model::printModelMatrix()
{
    checkIfInitialized();

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

glm::vec3 Model::getColor(){return color;}

void Model::setColor(ModelColor op) {
    switch (op) {
    case ModelColor::RED:
        color = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case ModelColor::GREEN:
        color = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case ModelColor::BLUE:
        color = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case ModelColor::YELLOW:
        color = glm::vec3(1.0f, 1.0f, 0.0f);
        break;
    case ModelColor::CYAN:
        color = glm::vec3(0.0f, 1.0f, 1.0f);
        break;
    case ModelColor::MAGENTA:
        color = glm::vec3(1.0f, 0.0f, 1.0f);
        break;
    case ModelColor::WHITE:
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        break;
    case ModelColor::BLACK:
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    }
}
