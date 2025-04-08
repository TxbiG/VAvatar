#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "shaderClass.h"
#include "Mesh.h"
//#include "Material.h"
//#include "Skeleton.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

//struct texture {
	//unsigned int id;
	//std::string type;
//};

/*
class Model
{
public:
	Model(const char* filename);

	void input(); // Set Skeletion Info
	void update(); // Set it to Skeletion
	void render(Shader& shader, Camera3D& camera); // Render Pose


	void set_BonesVisability(bool b) { isVisible = b; }
	bool get_bonesVisability() const { return isVisible; }

private:
	std::vector<Mesh> meshes;
    std::string directory;

	// Meshes
	void processNode(aiNode* node, const aiScene* scene);													// Process the File format using Assimp
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);											// Process the Mesh using Assimp

	// Textures
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const char* typeName);		// Process the Material Textures using Assimp


	// Bones
	void processSkeleton(aiMesh* mesh);																// Process the Skeletial bones using Assimp
	void process_single_Bone(int bone_index, const aiBone* bone);
	void drawLine(const glm::mat4& start, int Red, int Green, int Blue, GLuint width);        // Simple line rendering method

	bool isVisible = true;                                              // Toggle Visually display bones
};*/
#endif // MODEL_CLASS_H
