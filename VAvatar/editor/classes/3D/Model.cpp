#include "Model.h"

/*
// Init Model
Model::Model(const char* filename)
{
	Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene ||  scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;
        return;
    }

    //directory = filename.substr(0, filename.find_last_of('/'));
    aiNode* node = nullptr;
    processNode(node, scene);
}

// Input is used to get anchors from webcam and place them on Models bones
void Model::input() {}

// Update is anything like position, and bones
void Model::update() {}

// Render is rending mesh (draw)
void Model::render(Shader& shader, Camera3D& camera) 
{
	// Draw all the meshes from the FBX file
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].render(shader, camera, meshes[i].get_matrix(), meshes[i].get_translation(), meshes[i].get_rotation(), meshes[i].get_scale());
	}
}


void Model::processNode(aiNode* node, const aiScene* scene)
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


Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};
        glm::vec3 vector{}; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec{};
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    //processSkeleton(mesh);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const char* typeName)
{
    std::vector<Texture> texture;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < texture.size(); j++)
        {
            if(std::strcmp(texture[j].get_path(), str.C_Str()) == 0)
            {
                texture.push_back(texture[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            std::vector<Texture> tmp;
            tmp[i].set_name(str.C_Str());
            tmp[i].set_type(typeName);
            tmp[i].set_path(str.C_Str());
            texture.push_back(tmp[i]);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }

        return texture;
    }
}

void Model::drawLine(const glm::mat4& start, int Red, int Green, int Blue, GLuint width)
{
}

void Model::processSkeleton(aiMesh* mesh)
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

void Model::process_single_Bone(int bone_index, const aiBone* bone)
{
    for (int i = 0; i < bone->mNumWeights; i++)
    {
        const aiVertexWeight& vw = bone->mWeights[i];
    };

    return;
}*/