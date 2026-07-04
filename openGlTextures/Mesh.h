#pragma once
#define GLM_ENABLE_EXPERIMENTAL
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
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

struct RawFace {
    //raw strings
    std::string s1, s2, s3;
};
struct ThreadResult {
    std::vector<glm::vec3> vertices;
    std::vector<RawFace> faces;
};

struct Vertex {
    int idx = -1;
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    int out = -1;

    Vertex() = default;
    Vertex(int i, glm::vec3 p) : idx(i), pos(p), out(-1) {}
    void print() {
        std::cout << "Index: " << idx << std::endl;
        std::cout << "Posiiton: (" << pos.x  << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        std::cout << "Out Index: " << out;
    }
};

struct Face {
    int idx = -1;
    int boundary = -1;

    Face() = default;
    Face(int i, int b) : idx(i), boundary(b) {}
};

struct HalfEdge {
    int idx = -1;
    int origin = -1;
    int to = -1;
    int twin = -1;
    int next = -1;
    int prev = -1;
    int face = -1;

    HalfEdge() = default;
    HalfEdge(int id, int tw, int nx, int pr, int orig, int f, int t)
        : idx(id), twin(tw), next(nx), prev(pr), origin(orig), face(f), to(t) {
    }
};

using point = bg::model::point<float, 3, bg::cs::cartesian>;

//define the box that will be surrounding the points
using box = bg::model::box<point>;

//we will hold pairs of the vertex coordinates positions (raw floats) and a pointer to the actual vertex struct to minimize memory duplication
using value = std::pair<point, Vertex*>;

//type alias for the vertex tree
using vTree = bgi::rtree<value, bgi::quadratic<32>>;

// map of the existing records
// key: 64-bit integer made from the fromIdx and to toIdx pushed together
// value: edgeIdx
using edgeRecordMap = std::unordered_map<uint64_t, int>;
using vertexRecordMap = std::unordered_map<int, int>;
//a key made of combining the from and to integers in a halfedge int a 64-bit integer
using edgeKey = uint64_t;

class Mesh {
public:

    

    //storage containers
    //may have to switch to pointer-based implmentation later, as deleting/adding elements would break indices
    std::vector<Vertex> vertices;
    std::vector<HalfEdge> edges;
    std::vector<Face> faces;
    std::vector<unsigned int> indices;


    //r tree containing pointers to our vertices
    vTree vertexTree;
    vertexRecordMap vertexRecord;
    edgeRecordMap edgeRecord;

    //methods
    int createEdge(int from, int to);

    //create a triangle using the following indices for vertices
    //@param v0 index to the left vertex
    //@param v1 index to the middle vertex
    //@param v2 index to the right vertex
    void makeTriangle(int v0, int v1, int v2);

    //returns a 64 bit integer combining from and to
    //used for looking up keys in edgeRecord
    //@param from index of the starting vertex
    //@param to index of the vertex the edge points to
    edgeKey makeRecordKey(int from, int to);

    //returns an edge from edgeRecord
    //@param from index of the starting vertex
    //@param to index of the vertex the edge points to
    int getEdgeFromRecord(int from, int to);

    //checks if an edge starting at **from** and ending at **to** exists
    //@param from index of the starting vertex
    //@param to index of the vertex the edge points to
    bool hasEdge(int from, int to);

    //function that simultaneously fills up the vertices aray and creates a triangle with said vertices
    //@param mesh current mesh from assimp to iterate on
    void parseMesh(aiMesh* mesh);
    //recursively looks through child nodes to run parseMesh on
    void parseNode(aiNode* node, const aiScene* scene);

    //utility
    int getOrigin(HalfEdge& h);
    int getOut(Vertex& v);
    int getTo(HalfEdge& h);
    int getNext(HalfEdge& h);
    int getTwin(HalfEdge& h);
    int getPrev(HalfEdge& h);
    int getBoundary(Face& f);
    void printStructure();
    //void walkTest();
    void printVertices();
    void printIndices();
    void printVerticesAndIndices();
    //function for bulk instertion of vertices into the tree
    vTree bulkInsert(std::vector<Vertex>& vertices);
    Vertex* nearestVertex(float x, float y, float z);
    //constructor
    Mesh() = default;
    Mesh(std::string filename);
};