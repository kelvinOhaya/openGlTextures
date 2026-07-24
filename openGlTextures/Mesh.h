#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define _USE_MATH_DEFINES

#include <vector>
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <glm/gtx/hash.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

struct MyTraits : public OpenMesh::DefaultTraits {

    typedef OpenMesh::Vec4f Color;

    VertexAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::TexCoord2D| OpenMesh::Attributes::Status | OpenMesh::Attributes::Color);
    FaceAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status);
};



using myMesh = OpenMesh::TriMesh_ArrayKernelT<MyTraits>;

using point = bg::model::point<float, 3, bg::cs::cartesian>;

//define the box that will be surrounding the points
using box = bg::model::box<point>;

//we will hold pairs of the vertex coordinates positions (raw floats) and a pointer to the actual vertex struct to minimize memory duplication
using value = std::pair<point, myMesh::VertexHandle*>;

//type alias for the vertex tree
using vTree = bgi::rtree<value, bgi::quadratic<32>>;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    Vertex(glm::vec3 pos, glm::vec3 norm) : pos(pos), norm(norm) {};
};


class Mesh {
public:
    //storage containers
    //may have to switch to pointer-based implmentation later, as deleting/adding elements would break indices
    std::vector<unsigned int> indices;
    myMesh data;
    std::vector<myMesh::VertexHandle> vHandles;
    std::vector<myMesh::FaceHandle> fHandles;

    //constructor
    Mesh() = default;
    void parseMesh(aiMesh* mesh);
    void parseNode(aiNode* node, const aiScene* scene);
    Mesh(float* coords);
    Mesh(std::string filename);
    void printVertices();
    void printIndices();
};