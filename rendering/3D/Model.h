#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <fbxsdk.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//#include "shaderClass.h"
#include "Mesh.h"
#include "Material.h"
#include "Skeleton.h"
#include "../../editor/import/FBXLoader.h"

using namespace std;

/*
* Model Class is designed to convert FBX data to readable data for OpenGL and GLM.
* Tells Mesh about the Vertices, and indeces.
* Tells Material the values and textures that are used.
* Tells Skeleton the name, position, rotation, joints, and children-indices.
* Possibly get Shaders?!?
*/

// TODO - Find away to get matricesMeshes info from traverseNode() func get scale, roation, position translations

class Model
{
public:
	Model(const char* filename);
	~Model();

	void Draw();

	//void input(); // Set Skeletion Info
	//void update(); // Set it to Skeletion
	//void render(); // Render Pose

	std::vector<Vertex> vert;
	std::vector<GLuint> indi;
	std::vector<Texture> text;

	// All the meshes and transformations
	std::vector<Mesh> meshes;
	std::vector<glm::vec3> translationsMeshes;
	std::vector<glm::quat> rotationsMeshes;
	std::vector<glm::vec3> scalesMeshes;
	std::vector<glm::mat4> matricesMeshes;

private:
	std::vector<Material> materials;
	std::vector<Skeleton> bones;

	FBXLoader import;
};
#endif // MODEL_CLASS_H
