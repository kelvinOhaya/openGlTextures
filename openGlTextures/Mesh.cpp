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

	//push vertex indices into indices
	indices.push_back(v0);
	indices.push_back(v1);
	indices.push_back(v2);
	
	
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
	// Setup twin links if they haven't been linked to something else yet
	int t0 = edges[h0].twin;
	int t1 = edges[h1].twin;
	int t2 = edges[h2].twin;

	if (edges[t0].next == t0) {
		edges[t0].next = t2;
		edges[t0].prev = t1;
	}
	if (edges[t1].next == t1) {
		edges[t1].next = t0;
		edges[t1].prev = t2;
	}
	if (edges[t2].next == t2) {
		edges[t2].next = t1;
		edges[t2].prev = t0;
	}

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
	auto it = vertexRecord.find(pos);
	if (it != vertexRecord.end()) {
		return it->second;
	}


	int idx = vertices.size();
	Vertex v(idx, pos);
	vertices.push_back(v);
	vertexRecord.insert(std::make_pair( pos, idx ));
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

		if (from >= 0 && from < (int)vertices.size()) {
			vertices[from].out = halfEdgeIdx;
		}

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
	makeSampleDiamond();
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

void Mesh::makeSampleDiamond() {
	// First face flat on the front plane
	makeTriangle(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f));
	// Second face pushed slightly back in space (Z = -0.5f) so it has visible 3D dimension
	makeTriangle(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0), glm::vec3(2, 0, 0));
}

void Mesh::makeSpaceship() {
	// ==========================================
	// 1. MAIN HULL / FUSELAGE (Nose and Body)
	// ==========================================

	// Top Nose Cone Facets
	makeTriangle(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(-0.3f, -0.2f, 1.0f), glm::vec3(0.0f, 0.3f, 1.0f)); // Top Left
	makeTriangle(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.3f, 1.0f), glm::vec3(0.3f, -0.2f, 1.0f)); // Top Right

	// Bottom Nose Cone Facets
	makeTriangle(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.3f, -0.2f, 1.0f), glm::vec3(0.0f, -0.4f, 1.0f)); // Bottom Right
	makeTriangle(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, -0.4f, 1.0f), glm::vec3(-0.3f, -0.2f, 1.0f)); // Bottom Left

	// Fuselage Midsection (Upper Panels)
	makeTriangle(glm::vec3(-0.3f, -0.2f, 1.0f), glm::vec3(-0.4f, -0.2f, -1.5f), glm::vec3(0.0f, 0.3f, 1.0f));
	makeTriangle(glm::vec3(0.0f, 0.3f, 1.0f), glm::vec3(-0.4f, -0.2f, -1.5f), glm::vec3(0.0f, 0.4f, -1.5f));
	makeTriangle(glm::vec3(0.0f, 0.3f, 1.0f), glm::vec3(0.0f, 0.4f, -1.5f), glm::vec3(0.4f, -0.2f, -1.5f));
	makeTriangle(glm::vec3(0.0f, 0.3f, 1.0f), glm::vec3(0.4f, -0.2f, -1.5f), glm::vec3(0.3f, -0.2f, 1.0f));

	// Fuselage Midsection (Lower Panels)
	makeTriangle(glm::vec3(-0.3f, -0.2f, 1.0f), glm::vec3(0.0f, -0.4f, 1.0f), glm::vec3(-0.4f, -0.2f, -1.5f));
	makeTriangle(glm::vec3(0.0f, -0.4f, 1.0f), glm::vec3(0.0f, -0.5f, -1.5f), glm::vec3(-0.4f, -0.2f, -1.5f));
	makeTriangle(glm::vec3(0.0f, -0.4f, 1.0f), glm::vec3(0.4f, -0.2f, -1.5f), glm::vec3(0.0f, -0.5f, -1.5f));
	makeTriangle(glm::vec3(0.3f, -0.2f, 1.0f), glm::vec3(0.4f, -0.2f, -1.5f), glm::vec3(0.0f, -0.4f, 1.0f));

	// ==========================================
	// 2. RAISED COCKPIT CANOPY
	// ==========================================
	makeTriangle(glm::vec3(0.0f, 0.3f, 0.8f), glm::vec3(-0.2f, 0.2f, 0.0f), glm::vec3(0.0f, 0.6f, -0.3f)); // Windshield L
	makeTriangle(glm::vec3(0.0f, 0.3f, 0.8f), glm::vec3(0.0f, 0.6f, -0.3f), glm::vec3(0.2f, 0.2f, 0.0f)); // Windshield R
	makeTriangle(glm::vec3(0.0f, 0.6f, -0.3f), glm::vec3(-0.2f, 0.2f, 0.0f), glm::vec3(0.0f, 0.4f, -1.0f)); // Canopy Back L
	makeTriangle(glm::vec3(0.0f, 0.6f, -0.3f), glm::vec3(0.0f, 0.4f, -1.0f), glm::vec3(0.2f, 0.2f, 0.0f)); // Canopy Back R

	// ==========================================
	// 3. MAIN SWEPT WINGS (Left & Right)
	// ==========================================

	// Left Main Wing (Top & Bottom Facets for thickness)
	makeTriangle(glm::vec3(-0.35f, -0.1f, 0.2f), glm::vec3(-2.2f, -0.3f, -1.8f), glm::vec3(-0.4f, -0.1f, -1.5f)); // Top Flight Deck
	makeTriangle(glm::vec3(-0.35f, -0.1f, 0.2f), glm::vec3(-0.4f, -0.3f, -1.5f), glm::vec3(-2.2f, -0.3f, -1.8f)); // Underwing
	makeTriangle(glm::vec3(-2.2f, -0.3f, -1.8f), glm::vec3(-2.0f, -0.3f, -2.1f), glm::vec3(-0.4f, -0.1f, -1.5f)); // Trailing Edge Top
	makeTriangle(glm::vec3(-2.2f, -0.3f, -1.8f), glm::vec3(-0.4f, -0.3f, -1.5f), glm::vec3(-2.0f, -0.3f, -2.1f)); // Trailing Edge Bottom

	// Right Main Wing (Top & Bottom Facets for thickness)
	makeTriangle(glm::vec3(0.35f, -0.1f, 0.2f), glm::vec3(0.4f, -0.1f, -1.5f), glm::vec3(2.2f, -0.3f, -1.8f)); // Top Flight Deck
	makeTriangle(glm::vec3(0.35f, -0.1f, 0.2f), glm::vec3(2.2f, -0.3f, -1.8f), glm::vec3(0.4f, -0.3f, -1.5f)); // Underwing
	makeTriangle(glm::vec3(2.2f, -0.3f, -1.8f), glm::vec3(0.4f, -0.1f, -1.5f), glm::vec3(2.0f, -0.3f, -2.1f)); // Trailing Edge Top
	makeTriangle(glm::vec3(2.2f, -0.3f, -1.8f), glm::vec3(2.0f, -0.3f, -2.1f), glm::vec3(0.4f, -0.3f, -1.5f)); // Trailing Edge Bottom

	// ==========================================
	// 4. FORWARD WING CANARDS (Small Front Fins)
	// ==========================================
	makeTriangle(glm::vec3(-0.3f, -0.1f, 1.1f), glm::vec3(-0.9f, -0.15f, 0.6f), glm::vec3(-0.32f, -0.1f, 0.5f)); // Left Canard
	makeTriangle(glm::vec3(0.3f, -0.1f, 1.1f), glm::vec3(0.32f, -0.1f, 0.5f), glm::vec3(0.9f, -0.15f, 0.6f)); // Right Canard

	// ==========================================
	// 5. TWIN REAR VERTICAL STABILIZERS (Fins)
	// ==========================================

	// Left Tail Fin
	makeTriangle(glm::vec3(-0.25f, 0.2f, -1.0f), glm::vec3(-0.7f, 1.1f, -2.0f), glm::vec3(-0.25f, 0.2f, -1.8f)); // Outward Face
	makeTriangle(glm::vec3(-0.25f, 0.2f, -1.0f), glm::vec3(-0.25f, 0.2f, -1.8f), glm::vec3(-0.7f, 1.1f, -2.0f)); // Inward Face

	// Right Tail Fin
	makeTriangle(glm::vec3(0.25f, 0.2f, -1.0f), glm::vec3(0.25f, 0.2f, -1.8f), glm::vec3(0.7f, 1.1f, -2.0f)); // Outward Face
	makeTriangle(glm::vec3(0.25f, 0.2f, -1.0f), glm::vec3(0.7f, 1.1f, -2.0f), glm::vec3(0.25f, 0.2f, -1.8f)); // Inward Face

	// ==========================================
	// 6. ENGINE THRUSTER CAP (Engine Exhaust Plate)
	// ==========================================
	makeTriangle(glm::vec3(-0.4f, -0.2f, -1.5f), glm::vec3(0.4f, -0.2f, -1.5f), glm::vec3(0.0f, 0.4f, -1.5f)); // Upper Half
	makeTriangle(glm::vec3(-0.4f, -0.2f, -1.5f), glm::vec3(0.0f, -0.5f, -1.5f), glm::vec3(0.4f, -0.2f, -1.5f)); // Lower Half
}

void Mesh::printVertices()
{
	std::cout << "VERTICES:\n";
	for (auto& v : vertices) {
		std::cout << v.pos.x << ", " << v.pos.y << ", " << v.pos.z << std::endl;
	}
}

void Mesh::printIndices() {
	std::cout << "INDICES:\n";
	for (size_t it = 0;  auto& i : indices) {
		std::cout << i << (((it+1) % 3 == 0 && it > 0) ? "\n" : ", ");
		it+= 1;
	}
}

void Mesh::printVerticesAndIndices() {
	printVertices();
	printIndices();
}

Mesh::Mesh(std::string filename)
{
	if (filename.find(".obj") == std::string::npos) {throw std::runtime_error("must be a .obj file");}

	std::ifstream objFile(filename);
	std::string line;
	std::string word;
	std::vector<glm::vec3> temPosLst;


	while (std::getline(objFile, line)) {

		std::istringstream iss(line);
		std::vector<std::string> splitLine;

		while (iss >> word) {
			splitLine.push_back(word);
		}

		if (splitLine[0] == "v") {
			float x = std::stof(splitLine[1]);
			float y = std::stof(splitLine[2]);
			float z = std::stof(splitLine[3]);

			temPosLst.push_back(glm::vec3(x, y, z));
		}
		else if (splitLine[0] == "f") {
			//get indices (-1 bc indices in .obj are 1 indexed)
			unsigned int i1 = std::stoul(splitLine[1])-1;
			unsigned int i2 = std::stoul(splitLine[2])-1;
			unsigned int i3 = std::stoul(splitLine[3])-1;


			glm::vec3 p1 = temPosLst[i1];
			glm::vec3 p2 = temPosLst[i2];
			glm::vec3 p3 = temPosLst[i3];

			makeTriangle(p1, p2, p3);
		}
	}
	objFile.close();

	
}
