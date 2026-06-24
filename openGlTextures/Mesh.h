#pragma once

#include <vector>
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include <iostream>

class Mesh {
public:
    // ==========================================
    // DATA STRUCTS
    // ==========================================
    struct Vertex {
        int idx = -1;
        glm::vec3 pos = glm::vec3(0.0f);
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

    // ==========================================
    // STORAGE CONTAINERS
    // ==========================================
    std::vector<Vertex> vertices;
    std::vector<HalfEdge> edges;
    std::vector<Face> faces;

    // Explicitly using std::pair<int, int> as the key type
    std::map<std::pair<int, int>, int> edgeRecord;

    // ==========================================
    // CORE METHODS
    // ==========================================
    int createVertex(glm::vec3 pos);
    int createEdge(int from, int to);
    void makeTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2);
    HalfEdge& createHalfEdge(HalfEdge& twin, HalfEdge& prev, HalfEdge& next, Vertex& origin, Face& face, Vertex& to);

    int getEdgeFromRecord(int from, int to);
    bool hasEdge(int from, int to);

    // ==========================================
    // HELPERS & TESTING
    // ==========================================
    int getOrigin(HalfEdge& h);
    int getOut(Vertex& v);
    int getTo(HalfEdge& h);
    int getNext(HalfEdge& h);
    int getTwin(HalfEdge& h);
    int getPrev(HalfEdge& h);
    int getBoundary(Face& f);
    void printStructure();
    void walkTest();
};