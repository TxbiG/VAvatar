#include "Loader.h"

Loader::Loader(const char* filename) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene ||  scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error assimp: " << importer.GetErrorString() << endl;
        return;
    }

    directory = filename.substr(0, filename.find_last_of('/'));
    void processNode(aiNode* node, aiScene scene);
}

void Loader::processNode(aiNode* node, aiScene scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}
Mesh Loader::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates

        vertices.push_back(vertex);
    }
    // process indices

    // process material
    if(mesh->mMaterialIndex >= 0)
    {

    }

    return Mesh(vertices, indices, textures);
}