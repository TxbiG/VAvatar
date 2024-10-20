#include "Skeleton.h"

Skeleton::Skeleton(const std::string& name, const GLuint VertexID, const float Weight, const glm::mat4& offsetMatrix, Bones* parent)
{
    children.emplace_back(name, offsetMatrix, parent);
    if (parent != nullptr) {
        Bones* tmp{};
        tmp->name = name;
        tmp->VertexID = VertexID;
        tmp->Weight = Weight;
        tmp->parent = parent;
        children.push_back(tmp);
    }
}

void Skeleton::update(const std::string& name, const glm::mat4& offsetMatrix, Bones* parent)
{
    // update bones
}

void Skeleton::render(Shader& shader, Camera3D& camera) const
{
    // If true draw the bones on screen
    if (get_bonesVisability())
    {
        /*
        for (auto& bone : children) {
            drawLine(bone->offsetMatrix 255, 0,0, 4);
        }
        */
    }
}

void Skeleton::drawLine(const glm::mat4& start, int Red, int Green, int Blue, GLuint width) 
{
    //glLineWidth(width);
    //glDrawElements(GL_LINES, 1, 0, 0);
        //glColor3fv((Red / 255, Green / 255, Blue / 255));
        //glVertex3f(start.x, start.y, start.z);
        //glVertex3f(end.x, end.y, end.z);
    //glEnd();
}

void Skeleton::processSkeleton(aiMesh* mesh)
{
    if (mesh->HasBones())
    {
        for (int i = 0; i < mesh->mNumBones; i++)
        {
            process_single_Bone(i, mesh->mBones[i]);
        };
    }

    return;
}

void Skeleton::process_single_Bone(int bone_index, const aiBone* bone)
{
    for (int i = 0; i < bone->mNumWeights; i++)
    {
        const aiVertexWeight& vw = bone->mWeights[i];
    };

    return;
}