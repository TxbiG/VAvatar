#include "Skeleton.h"

Skeleton::Skeleton(const Bone& bone)
{
    bones.push_back(bone);
    boneTransforms.push_back(glm::mat4(1.0f)); // Initialize with identity matrix
}

void Skeleton::updateBoneTransforms()
{
    for (auto& bone : bones) {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), bone.position);
        glm::mat4 rotation = glm::toMat4(bone.rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), bone.scale);
        boneTransforms[bone.id] = translation * rotation * scale;
    }
}

void Skeleton::get_displayBones() const
{
    if (isVisible)
    {

    }
    else
    {

    }
}
