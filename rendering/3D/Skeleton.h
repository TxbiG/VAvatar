#ifndef SKELETON_CLASS_H
#define SKELETON_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/fwd.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <string>
#include <vector>

using namespace std;

// Might have to possibly switch from IK and FK and visversa?

struct Bone {
    // Bone Data
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    std::string name;
    int id;

    // initialization bones
    Bone(const std::string& name, int id, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : name(name), id(id), position(position), rotation(rotation), scale(scale) {}
};

class Skeleton
{
public:
    Skeleton(const Bone& bone);

    void updateBoneTransforms();

    bool set_displayBones(bool b) { return isVisible = b; }
    void get_displayBones() const;

private:
    bool isVisible = { true }; // Visually display bones or not

    std::vector<Bone> bones; // List of bones in the skeleton
    std::vector<glm::mat4> boneTransforms; // Transformation matrices for each bone
};
#endif // SKELETON_CLASS_H