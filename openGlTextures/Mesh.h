#pragma once

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

class Mesh {
public:
    //structs

    //less than struct that allows us to use vec3 as a key in vertexRecord, since vec3 doesn't have a less than operator
    struct Vec3Less {
        bool operator()(const glm::vec3& a, const glm::vec3& b) const {
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            return a.z < b.z;
        }
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
    std::map<std::pair<int, int>, int> edgeRecord;

    //map of existing vertices
    std::map<glm::vec3, int, Vec3Less> vertexRecord;


    //a record of indices which will be used for flattening
    std::vector<unsigned int> indices;

    //methods
    //creates a vertex if one doesnt exist already
    int createVertex(glm::vec3 pos);
    int createEdge(int from, int to);
    void makeTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2);
    HalfEdge& createHalfEdge(HalfEdge& twin, HalfEdge& prev, HalfEdge& next, Vertex& origin, Face& face, Vertex& to);

    int getEdgeFromRecord(int from, int to);
    bool hasEdge(int from, int to);

    //utility
    int getOrigin(HalfEdge& h);
    int getOut(Vertex& v);
    int getTo(HalfEdge& h);
    int getNext(HalfEdge& h);
    int getTwin(HalfEdge& h);
    int getPrev(HalfEdge& h);
    int getBoundary(Face& f);
    void printStructure();
    void walkTest();
    void makeSampleDiamond();
    void makeSpaceship();
    void printVertices();
    void printIndices();
    void printVerticesAndIndices();

    //constructor
    Mesh() = default;
    Mesh(std::string filename);
};