#ifndef SKELETON_CLASS_H
#define SKELETON_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <string>
#include <vector>
#include "../3D/shaderClass.h"
#include "Camera3D.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
//#include "../../Bone3D.h"

struct Bones
{
    std::string name;
    GLuint VertexID;
    float Weight;

    Bones* parent; // Parent bone
};

class Skeleton
{
public:
    Skeleton(const std::string& name, const GLuint VertexID, const float Weight, const glm::mat4& offsetMatrix, Bones* parent);

    void update(const std::string& name, const glm::mat4& offsetMatrix, Bones* parent);
    void render(Shader& shader, Camera3D& camera) const;

    void set_BonesVisability(bool b) { isVisible = b; }
    bool get_bonesVisability() const { return isVisible; }

private:
    // Simple line rendering method
    void drawLine(const glm::mat4& start, int Red, int Green, int Blue, GLuint width);        // Simple line rendering method

    void processSkeleton(aiMesh* mesh);														  // Process the Skeletial bones using Assimp
    void process_single_Bone(int bone_index, const aiBone* bone);                             // Process each bone

    std::vector<Bones*> children;                                       // Children bones
    bool isVisible = true;                                              // Toggle Visually display bones
};
#endif // SKELETON_CLASS_H