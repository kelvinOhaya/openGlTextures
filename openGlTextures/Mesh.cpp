#include "Mesh.h"
#include <thread>
#include <future>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <chrono>
#include "Globals.h"

void Mesh::parseMesh(aiMesh* mesh)
{
	unsigned int globalSize = vHandles.size();
	//std::cout << mesh->mNumVertices << std::endl;
	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		//get values from assimp
		     glm::vec3 v(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		OpenMesh::Vec3f vf(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		     glm::vec3 n(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		glm::vec2 tex;
		
		//add the values to the vertex handle from openMesh
		myMesh::VertexHandle vh = data.add_vertex(myMesh::Point(v.x, v.y, v.z));
		data.set_normal(vh, myMesh::Normal(n.x, n.y, n.z));

		if (mesh->mTextureCoords[0]) {
			tex = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			data.set_texcoord2D(vh, myMesh::TexCoord2D(tex.x, tex.y));
		}

		//add vertex handle to vector of vHandles;
		vHandles.push_back(vh);
	}
	int currentSize = indices.size();

	//get all indices from the faces
	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		myMesh::FaceHandle fHandle;
		
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(globalSize + face.mIndices[j]);
		};
	}
	//std::cout << "mNumIndices = " << currentSize + ( mesh->mNumFaces * 3) << std::endl;
	//std::cout << "indices.size() = " << indices.size() << std::endl;
	//std::cout << std::endl;
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

Mesh::Mesh(float* coords)
{

}

//DRAGON INFO:
// - 435545 vertices
// - 2613918 indices
Mesh::Mesh(std::string filename) {

	auto startTime = std::chrono::high_resolution_clock::now();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality|aiProcess_FlipUVs | aiProcess_SortByPType);

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

	

	parseNode(scene->mRootNode, scene);

	// Get the largest containing box of the entire R-tree
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	// 4. Print the performance metrics to the console
	std::cout << "\n========================================\n";
	std::cout << "Successfully parsed " << vHandles.size() << " vertices.\n";
	std::cout << "Drawing " << indices.size() << " indices.\n";
	std::cout << "MESH GENERATION TIME: " << duration << " ms (" << (duration / 1000.0f) << " seconds)\n";
	std::cout << "========================================\n\n";
}

void Mesh::printVertices()
{
	for (int i = 0; i < vHandles.size(); i++) {
        //push points into the row
        myMesh::VHandle vh = vHandles[i];
        myMesh::Point pos = data.point(vh);
        myMesh::Normal norm = data.normal(vh);

        std::cout << "Position: " << pos << std::endl;
        std::cout << "Normal: " << norm << std::endl;
        std::cout << std::endl;
    }
}

void Mesh::printIndices()
{
	std::cout << "INDICES:" << std::endl;
	for (int i = 0; i < indices.size(); i++) {
		std::cout << indices[i] << "," <<((i+1)%3 == 0? "\n":" ");
	}
}


