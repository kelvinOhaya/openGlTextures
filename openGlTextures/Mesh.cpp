#include "Mesh.h"
#include <thread>
#include <future>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <chrono>
#include "Globals.h"


//helper data functions
int Mesh::getOrigin(HalfEdge& h) { return vertices[h.origin].idx; }
int Mesh::getOut(Vertex& v) { return edges[v.out].idx; }
int Mesh::getTo(HalfEdge& h) { return vertices[h.to].idx; }
int Mesh::getNext(HalfEdge& h) { return edges[h.next].idx; }
int Mesh::getTwin(HalfEdge& h) { return edges[h.twin].idx; }
int Mesh::getPrev(HalfEdge& h) { return edges[h.prev].idx; }
int Mesh::getBoundary(Face& f) { return edges[f.boundary].idx; }



edgeKey Mesh::makeRecordKey(int from, int to) {
	// BUG: >> 32 shifts 'from' completely out of existence into 0
	uint64_t recordKey = (static_cast<uint64_t>(from)) << 32 | static_cast<uint64_t>(to); // FIX: Use << 32
	return recordKey;
}

int Mesh::getEdgeFromRecord(int from, int to)
{
	uint64_t recordKey = (static_cast<uint64_t>(from)) >> 32 | static_cast<uint64_t>(to);
	//find the edge with the stored index at the appropriate map
	return edgeRecord.at(recordKey);

}

bool Mesh::hasEdge( int from,  int to){
	return (edgeRecord.find(makeRecordKey(from,to)) != edgeRecord.end());
}

vTree Mesh::bulkInsert(std::vector<Vertex>& vertices)
{
	std::vector<value> points;
	points.reserve(vertices.size());
	for (const auto& vertex : vertices) {
		point treePos(vertex.pos.x, vertex.pos.y, vertex.pos.z);
		points.push_back({ treePos, const_cast<Vertex*>(&vertex) });
	}
	return vTree(points.begin(), points.end());
}

Vertex* Mesh::nearestVertex(float x, float y, float z)
{
	std::vector<value> result;
	point p(x,y,z);
	bgi::query(vertexTree, bgi::nearest(p, 1), std::back_inserter(result));
	return result[0].second;
}

int Mesh::createEdge( int from,  int to)
{
	uint64_t edgeKey = (static_cast<uint64_t>(from) << 32) | (static_cast<uint64_t>(to));
	
	auto it = edgeRecord.find(edgeKey);
	if (it != edgeRecord.end()) {
		int existingIdx = it->second;
		if (edges[existingIdx].face == -1) {
			return existingIdx;
		}
	}

	uint64_t twinKey = (static_cast<uint64_t>(to) << 32) | (static_cast<uint64_t>(from));
	
	//in the ardirection, the twin will be after the edge. for now the edges will have themselves as next and prev until made otherwise
	int halfEdgeIdx = edges.size();
	int twinIdx = halfEdgeIdx + 1;
		
	HalfEdge halfEdge(halfEdgeIdx, twinIdx, twinIdx, twinIdx, from,-1, to);
	HalfEdge twin(twinIdx, halfEdgeIdx, halfEdgeIdx, halfEdgeIdx, to, -1, from);

	edges.push_back(halfEdge);
	edges.push_back(twin);

	edgeRecord[edgeKey] = halfEdge.idx;
	edgeRecord[twinKey] = twin.idx;

	if (from >= 0 && from < (int)vertices.size()) {
		vertices[from].out = halfEdgeIdx;
	}

	return halfEdgeIdx;
	

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

//void Mesh::walkTest() {
//	std::cout << "\n==================================================\n";
//	std::cout << "          STARTING TOPOLOGY LOOP WALK TEST          \n";
//	std::cout << "==================================================\n";
//
//	// 1. Generate the geometry
//
//	// Print the full asset table so we can cross-reference indices visually
//	printStructure();
//
//	// 2. Select our launching point (Vertex 2)
//	int startVertexIdx = 0;
//	Vertex startVertex = vertices[startVertexIdx];
//
//	std::cout << "\n[STEP 1: THE ACCELERATOR]" << std::endl;
//	std::cout << " -> Standing on Vertex [" << startVertexIdx << "]" << std::endl;
//	std::cout << " -> Checking its single outgoing anchor... out = " << startVertex.out << std::endl;
//
//	if (startVertex.out == -1) {
//		std::cout << " [!] ERROR: Vertex has no outgoing half-edge! Loop broken.\n";
//		return;
//	}
//
//	// 3. Step onto the first road
//	int e0Idx = startVertex.out;
//	HalfEdge e0 = edges[e0Idx];
//	std::cout << " -> Stepped onto HalfEdge [" << e0Idx << "]" << std::endl;
//	std::cout << "    |-- Origin Vertex: " << e0.origin << std::endl;
//	std::cout << "    |-- Target Vertex (to): " << e0.to << std::endl;
//	std::cout << "    |-- Connected Face: " << e0.face << std::endl;
//	std::cout << "    |-- Next Edge Pointer: " << e0.next << std::endl;
//
//	// 4. Follow pointer to the second road
//	std::cout << "\n[STEP 2: FOLLOWING THE TRAIL]" << std::endl;
//	int e1Idx = e0.next;
//	HalfEdge e1 = edges[e1Idx];
//	std::cout << " -> Advanced to Next HalfEdge [" << e1Idx << "]" << std::endl;
//	std::cout << "    |-- Origin Vertex: " << e1.origin << std::endl;
//	std::cout << "    |-- Target Vertex (to): " << e1.to << std::endl;
//	std::cout << "    |-- Next Edge Pointer: " << e1.next << std::endl;
//
//	// 5. Follow pointer to the third road
//	std::cout << "\n[STEP 3: COMPLETING THE CIRCUIT]" << std::endl;
//	int e2Idx = e1.next;
//	HalfEdge e2 = edges[e2Idx];
//	std::cout << " -> Advanced to Next HalfEdge [" << e2Idx << "]" << std::endl;
//	std::cout << "    |-- Origin Vertex: " << e2.origin << std::endl;
//	std::cout << "    |-- Target Vertex (to): " << e2.to << std::endl;
//	std::cout << "    |-- Next Edge Pointer: " << e2.next << " (Should point back to Step 1!)" << std::endl;
//
//	// 6. Verify full loop closure
//	std::cout << "\n[STEP 4: INTEGRITY CHECK]" << std::endl;
//	if (e2.next == e0Idx) {
//		std::cout << " [SUCCESS] Circuit closed successfully! The path cleanly looped back to HalfEdge [" << e0Idx << "].\n";
//	}
//	else {
//		std::cout << " [CRITICAL FAILURE] Geometry broken! Edge [" << e2Idx << "] points to [" << e2.next << "] instead of [" << e0Idx << "].\n";
//	}
//
//	// 7. Check the twin configurations (The boundary check)
//	std::cout << "\n[STEP 5: TWIN EXCLUSION DATA]" << std::endl;
//	std::cout << " -> HalfEdge [" << e0Idx << "] has Twin [" << e0.twin << "]" << std::endl;
//	std::cout << " -> Twin [" << e0.twin << "] Next pointer is: " << edges[e0.twin].next << std::endl;
//	std::cout << " -> Twin [" << e0.twin << "] Prev pointer is: " << edges[e0.twin].prev << std::endl;
//
//	std::cout << "==================================================\n\n";
//}

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

void Mesh::printTreeMetrics()
{
	if (!vertexTree.empty()) {
		box bounding_box = vertexTree.bounds();
		std::cout << "Max containing box corners: " << std::endl;
		std::cout << "Min corner: " << bg::get<0>(bounding_box.min_corner()) << ", "
			<< bg::get<1>(bounding_box.min_corner()) << std::endl;
		std::cout << "Max corner: " << bg::get<0>(bounding_box.max_corner()) << ", "
			<< bg::get<1>(bounding_box.max_corner()) << std::endl;
	}
	else {
		std::cout << "The R-tree is empty." << std::endl;
	}
}

float Mesh::pointIntersectedWithBox(glm::vec3& direction, glm::vec3& origin) {
	//Smit's method will be used to determine box intersection
	//More on Smit's method: https://people.csail.mit.edu/amy/papers/box-jgt.pdf
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	glm::vec3 invRay(1 / direction.x, 1 / direction.y, 1 / direction.z);
	box bounds = vertexTree.bounds();

	//get the minimum and maximum bounds
	glm::vec3 minCorner(bg::get < bg::min_corner, 0>(bounds), bg::get <bg::min_corner, 1>(bounds), bg::get < bg::min_corner, 2 >(bounds));
	glm::vec3 maxCorner(bg::get<bg::max_corner, 0>(bounds), bg::get<bg::max_corner, 1>(bounds), bg::get<bg::max_corner, 2>(bounds));

	glm::vec3 boundsContainer[2] = { minCorner, maxCorner };
	//get the signed bit of the ends of each number instead of checking whether it is greater than zero or not
	//we could just use if statements, but this is a pretty cool optimization imo
	int signX = std::signbit(direction.x); //get the signed bit
	int signY = std::signbit(direction.y);

	//compute distances from direction to axis
	tmin = (boundsContainer[signX].x - origin.x) * invRay.x;
	tmax = (boundsContainer[1 - signX].x - origin.x) * invRay.x;

	tymin = (boundsContainer[signY].y - origin.y) * invRay.y;
	tymax = (boundsContainer[1 - signY].y - origin.y) * invRay.y;

	//end function early for direction passes that cant lead to intersection (eg. entering x-axis after already entering and exiting y axis)
	if ((tmin > tymax) || (tymin > tmax)) {
		// FIX: Return -1.0f instead of false (0.0f)
		return -1.0f;
	}

	//update boundaries to contain both intervals
	if (tymin > tmin) { tmin = tymin; }
	if (tymax < tmax) { tmax = tymax; }

	//compute distances from direction to z axis
	int signZ = std::signbit(direction.z);
	tzmin = (boundsContainer[signZ].z - origin.z) * invRay.z;
	tzmax = (boundsContainer[1 - signZ].z - origin.z) * invRay.z;

	if ((tmin > tzmax) || (tzmin > tmax)) {
		// FIX: Return -1.0f instead of false (0.0f)
		return -1.0f;
	}

	//update boundaries to contian all intervals
	if (tzmin > tmin) { tmin = tzmin; }
	if (tzmax < tmax) { tmax = tzmax; }

	// FIX: Only evaluate tmin against visible space, remove strict tmax limit
	if (tmin >= 0.0f && tmin > Globals::NEAR && tmin < Globals::FAR) {
		return tmin;
	}
	else {
		return -1.0f;
	}
}

void Mesh::makeTriangle(int v0, int v1, int v2)
{
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
	int t0 = edges[h0].twin;
	int t1 = edges[h1].twin;
	int t2 = edges[h2].twin;

	// Only assign boundaries if the twin is actually open and belongs to an unassigned face layer
	if (edges[t0].next == t0 && edges[t0].face == -1) {
		edges[t0].next = t2;
		edges[t0].prev = t1;
	}
	if (edges[t1].next == t1 && edges[t1].face == -1) {
		edges[t1].next = t0;
		edges[t1].prev = t2;
	}
	if (edges[t2].next == t2 && edges[t2].face == -1) {
		edges[t2].next = t1;
		edges[t2].prev = t0;
	}

}

void Mesh::parseMesh(aiMesh* mesh)
{
	size_t baseVertexOffset = vertices.size();


	std::vector<OpenMeshTriangleMesh::VertexHandle> vHandles;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		int globalIdx = baseVertexOffset + i;

		glm::vec3 v(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec3 n(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		
		Vertex vert(globalIdx, v);
		vert.normal = n;
		vertices.push_back(vert);

		vHandles.push_back(omMesh.add_vertex(OpenMeshTriangleMesh::Point(v.x,v.y,v.z)));
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		if (face.mNumIndices < 3) continue;
		
		//get the indexed position of the vertex face.mIndices[n] is pointing to
		//used for making/finding keys in edgeRecord (for the from,to arguments)
		unsigned int globalIdx0 = baseVertexOffset + face.mIndices[0];
		unsigned int globalIdx1 = baseVertexOffset + face.mIndices[1];
		unsigned int globalIdx2 = baseVertexOffset + face.mIndices[2];

		this->indices.push_back(globalIdx0);
		this->indices.push_back(globalIdx1);
		this->indices.push_back(globalIdx2);

		// Map them to your OpenMesh vertex handles and add the face
		std::vector<OpenMeshTriangleMesh::VertexHandle> faceVHandles;
		faceVHandles.push_back(vHandles[face.mIndices[0]]);
		faceVHandles.push_back(vHandles[face.mIndices[1]]);
		faceVHandles.push_back(vHandles[face.mIndices[2]]);

		omMesh.add_face(faceVHandles);
	}
}

void Mesh::parseNode(aiNode* node, const aiScene *scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		parseMesh(mesh);
	}
	for (size_t i = 0; i < node->mNumChildren; i++) {
		parseNode(node->mChildren[i], scene);
	}
}

//DRAGON INFO:
// - 435545 vertices
// - 2613918 indices
Mesh::Mesh(std::string filename) {

	auto startTime = std::chrono::high_resolution_clock::now();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode) {
		std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
		return;
	}
	std::string directory = filename.substr(0, filename.find_last_of('/'));
	
	size_t totalVertices = 0;
	size_t totalFaces = 0;

	for (size_t i = 0; i < scene->mNumMeshes; ++i) {
		totalVertices += scene->mMeshes[i]->mNumVertices;
		totalFaces += scene->mMeshes[i]->mNumFaces;
	}

	vertices.reserve(totalVertices);
	faces.reserve(totalFaces);
	indices.reserve(totalFaces * 3);
	edges.reserve(totalFaces * 3);

	vertexRecord.reserve(totalVertices * 2);
	edgeRecord.reserve(totalFaces * 3 * 2);

	parseNode(scene->mRootNode, scene);
	vertexTree = bulkInsert(vertices);

	// Get the largest containing box of the entire R-tree
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	// 4. Print the performance metrics to the console
	std::cout << "\n========================================\n";
	std::cout << "Successfully parsed " << vertices.size() << " vertices.\n";
	std::cout << "Drawing " << indices.size() << " indices.\n";
	std::cout << "MESH GENERATION TIME: " << duration << " ms (" << (duration / 1000.0f) << " seconds)\n";
	std::cout << "========================================\n\n";
}


