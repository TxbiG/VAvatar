#pragma once

#include <fbxsdk.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include<glm/glm.hpp>
#include<glad/glad.h>

#include "../../rendering/3D/VBO.h"
#include "../../rendering/3D/Texture.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif


// Extract FBX data such as, Mesh(s), Texture(s)/Material(s) along with Bones

class FBXLoader
{
public:
	bool init(const char* filename);
	void ProcessNode(FbxNode* pNode);

	std::vector<Vertex> vert;
	std::vector<GLuint> indi;
	std::vector<Texture> text;

	FbxManager* SdkManager = { nullptr };
	FbxIOSettings* ios = { nullptr };
	FbxImporter* lImporter = { nullptr };
	//FbxScene*
	//FbxSkeleton*
};

