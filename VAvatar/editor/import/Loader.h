#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../classes/3D/Mesh.h"
#include "../classes/3D/Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

struct Face {
    int vIndex[3];
    int tIndex[3];
    int nIndex[3];
};

struct Bone {
    std::string name;
    glm::mat4 offsetMatrix;
};

struct Joint {
    std::string name;
    glm::vec3 position;
    glm::quat rotation;
    std::vector<std::pair<int, float>> weights;  // Vertex index and weight
};

class Loader
{
public:
    Loader(const char* filename);

    void processNode(aiNode* node, aiScene scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    
	// Mesh
    std::vector<Vertex> vert;
    std::vector<GLuint> indi;
    std::vector<Texture> text;


	// Matrix
    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationsMeshes;
    std::vector<glm::vec3> scalesMeshes;
    std::vector<glm::mat4> matricesMeshes;

	// Skeleton
	std::vector<Bone> bones;
    std::vector<Joint> joints;

private:
    std::vector<Vertex> normals;
};
#endif // LOADER_H
