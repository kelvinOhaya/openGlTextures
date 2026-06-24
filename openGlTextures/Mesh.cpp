#include "Mesh.h"


//helper data functions
int Mesh::getOrigin(HalfEdge& h) { return vertices[h.origin].idx; }
int Mesh::getOut(Vertex& v) { return edges[v.out].idx; }
int Mesh::getTo(HalfEdge& h) { return vertices[h.to].idx; }
int Mesh::getNext(HalfEdge& h) { return edges[h.next].idx; }
int Mesh::getTwin(HalfEdge& h) { return edges[h.twin].idx; }
int Mesh::getPrev(HalfEdge& h) { return edges[h.prev].idx; }
int Mesh::getBoundary(Face& f) { return edges[f.boundary].idx; }

void Mesh::makeTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2)
{
	int edgeStart = edges.size();
	int faceStart = faces.size();
	int vertexStart = vertices.size();
	
	int v0 = createVertex(pos0);
	int v1 = createVertex(pos1);
	int v2 = createVertex(pos2);
	
	int h0 = createEdge(v0, v1);
	int h1 = createEdge(v1, v2);
	int h2 = createEdge(v2, v0);
	
	int faceIdx = faces.size();
	Face f(faceIdx, h0);
	faces.push_back(f);

	edges[h0].face = faceIdx;
	edges[h1].face = faceIdx;
	edges[h2].face = faceIdx;

	// connecting half edges
	edges[h0].next = edges[h1].idx;
	edges[h0].prev = edges[h2].idx;

	edges[h1].next = edges[h2].idx;
	edges[h1].prev = edges[h0].idx;

	edges[h2].next = edges[h0].idx;
	edges[h2].prev = edges[h1].idx;

	//connecting twins
	edges[edges[h0].twin].next = edges[edges[h2].twin].idx;
	edges[edges[h0].twin].prev = edges[edges[h1].twin].idx;

	edges[edges[h1].twin].next = edges[edges[h0].twin].idx;
	edges[edges[h1].twin].prev = edges[edges[h2].twin].idx;

	edges[edges[h2].twin].next = edges[edges[h1].twin].idx;
	edges[edges[h2].twin].prev = edges[edges[h0].twin].idx;

}



Mesh::HalfEdge& Mesh::createHalfEdge(HalfEdge& twin, HalfEdge& prev, HalfEdge& next, Vertex& origin, Face& face, Vertex& to)
{
	int idx = edges.size();
	origin.out = idx;
	HalfEdge h = HalfEdge(idx, twin.idx, prev.idx, next.idx, origin.idx, face.idx, to.idx);
	edges.push_back(h);
	edgeRecord.insert({ {origin.idx, to.idx}, h.idx });

	return edges.back();
}

int Mesh::getEdgeFromRecord(int from, int to)
{
	//find the edge with the stored index at the appropriate map
	return edgeRecord.at({ from, to });

}

bool Mesh::hasEdge( int from,  int to){return (edgeRecord.find({ from,to }) != edgeRecord.end());}

int Mesh::createVertex(glm::vec3 pos)
{
	int idx = vertices.size();
	Vertex v(idx, pos);
	vertices.push_back(v);
	return v.idx;
}



int Mesh::createEdge( int from,  int to)
{
	if (hasEdge(from, to)) {
		return getEdgeFromRecord(from, to);
	}
	else {
		//in the array, the twin will be after the edge. for now the edges will have themselves as next and prev until made otherwise
		int halfEdgeIdx = edges.size();
		int twinIdx = halfEdgeIdx + 1;
		
		HalfEdge halfEdge(halfEdgeIdx, twinIdx, twinIdx, twinIdx, from,-1, to);
		HalfEdge twin(twinIdx, halfEdgeIdx, halfEdgeIdx, halfEdgeIdx, to, -1, from);

		edges.push_back(halfEdge);
		edges.push_back(twin);

		edgeRecord.insert({ {from,to}, halfEdge.idx});
		edgeRecord.insert({ {to, from}, twin.idx });

		vertices[to].out = halfEdgeIdx;

		return halfEdgeIdx;
	}

}

void Mesh::printStructure() {
	std::cout << "========= MESH TOPOLOGY REPORT =========\n\n";

	std::cout << "--- VERTICES ---\n";
	for (size_t i = 0; i < vertices.size(); ++i) {
		std::cout << "Vertex [" << i << "] -> Outgoing Edge: " << vertices[i].out << "\n";
	}

	std::cout << "\n--- HALF-EDGES ---\n";
	std::cout << "Idx | Origin | Twin | Next | Prev | Face\n";
	std::cout << "----------------------------------------\n";
	for (size_t i = 0; i < edges.size(); ++i) {
		printf("%3zu | %6d | %4d | %4d | %4d | %4d\n",
			i,
			edges[i].origin,
			edges[i].twin,
			edges[i].next,
			edges[i].prev,
			edges[i].face
		);
	}

	std::cout << "\n--- FACES ---\n";
	for (size_t i = 0; i < faces.size(); ++i) {
		std::cout << "Face [" << i << "] -> Inner Edge Boundary: " << faces[i].boundary << "\n";
	}
	std::cout << "\n========================================\n";
}


void Mesh::walkTest() {
	std::cout << "\n==================================================\n";
	std::cout << "          STARTING TOPOLOGY LOOP WALK TEST          \n";
	std::cout << "==================================================\n";

	// 1. Generate the geometry
	makeTriangle(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f));
	makeTriangle(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f));

	// Print the full asset table so we can cross-reference indices visually
	printStructure();

	// 2. Select our launching point (Vertex 2)
	int startVertexIdx = 0;
	Vertex startVertex = vertices[startVertexIdx];

	std::cout << "\n[STEP 1: THE ACCELERATOR]" << std::endl;
	std::cout << " -> Standing on Vertex [" << startVertexIdx << "]" << std::endl;
	std::cout << " -> Checking its single outgoing anchor... out = " << startVertex.out << std::endl;

	if (startVertex.out == -1) {
		std::cout << " [!] ERROR: Vertex has no outgoing half-edge! Loop broken.\n";
		return;
	}

	// 3. Step onto the first road
	int e0Idx = startVertex.out;
	HalfEdge e0 = edges[e0Idx];
	std::cout << " -> Stepped onto HalfEdge [" << e0Idx << "]" << std::endl;
	std::cout << "    |-- Origin Vertex: " << e0.origin << std::endl;
	std::cout << "    |-- Target Vertex (to): " << e0.to << std::endl;
	std::cout << "    |-- Connected Face: " << e0.face << std::endl;
	std::cout << "    |-- Next Edge Pointer: " << e0.next << std::endl;

	// 4. Follow pointer to the second road
	std::cout << "\n[STEP 2: FOLLOWING THE TRAIL]" << std::endl;
	int e1Idx = e0.next;
	HalfEdge e1 = edges[e1Idx];
	std::cout << " -> Advanced to Next HalfEdge [" << e1Idx << "]" << std::endl;
	std::cout << "    |-- Origin Vertex: " << e1.origin << std::endl;
	std::cout << "    |-- Target Vertex (to): " << e1.to << std::endl;
	std::cout << "    |-- Next Edge Pointer: " << e1.next << std::endl;

	// 5. Follow pointer to the third road
	std::cout << "\n[STEP 3: COMPLETING THE CIRCUIT]" << std::endl;
	int e2Idx = e1.next;
	HalfEdge e2 = edges[e2Idx];
	std::cout << " -> Advanced to Next HalfEdge [" << e2Idx << "]" << std::endl;
	std::cout << "    |-- Origin Vertex: " << e2.origin << std::endl;
	std::cout << "    |-- Target Vertex (to): " << e2.to << std::endl;
	std::cout << "    |-- Next Edge Pointer: " << e2.next << " (Should point back to Step 1!)" << std::endl;

	// 6. Verify full loop closure
	std::cout << "\n[STEP 4: INTEGRITY CHECK]" << std::endl;
	if (e2.next == e0Idx) {
		std::cout << " [SUCCESS] Circuit closed successfully! The path cleanly looped back to HalfEdge [" << e0Idx << "].\n";
	}
	else {
		std::cout << " [CRITICAL FAILURE] Geometry broken! Edge [" << e2Idx << "] points to [" << e2.next << "] instead of [" << e0Idx << "].\n";
	}

	// 7. Check the twin configurations (The boundary check)
	std::cout << "\n[STEP 5: TWIN EXCLUSION DATA]" << std::endl;
	std::cout << " -> HalfEdge [" << e0Idx << "] has Twin [" << e0.twin << "]" << std::endl;
	std::cout << " -> Twin [" << e0.twin << "] Next pointer is: " << edges[e0.twin].next << std::endl;
	std::cout << " -> Twin [" << e0.twin << "] Prev pointer is: " << edges[e0.twin].prev << std::endl;

	std::cout << "==================================================\n\n";
}

