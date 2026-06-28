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

class Mesh {
public:
    
    //less than struct that allows us to use vec3 as a key in vertexRecord, since vec3 doesn't have a less than operator
    struct Vec3Less {
        bool operator()(const glm::vec3& a, const glm::vec3& b) const {
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            return a.z < b.z;
        }
    };

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

    //storage containers
    std::vector<Vertex> vertices;
    std::vector<HalfEdge> edges;
    std::vector<Face> faces;

    // map of the existing records
    //{{fromIdx, toIdx}, edgeIdx}
    std::unordered_map<uint64_t, int> edgeRecord;

    //unordered_map of existing vertices
    std::unordered_map<int, int> vertexRecord;


    //a record of indices which will be used for flattening
    std::vector<unsigned int> indices;

    //methods
    //creates a vertex if one doesnt exist already
    int createVertex(int recordIdx, glm::vec3 pos, glm::vec3 normal);
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
    uint64_t makeRecordKey(int from, int to);

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
    //recursively looks through chi
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

    //constructor
    Mesh() = default;
    Mesh(std::string filename);
};