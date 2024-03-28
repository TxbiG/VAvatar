#include "FBXLoader.h"
#include <glm/fwd.hpp>

bool FBXLoader::init(const char* filename)
{
	// Initialize the SDK manager
	SdkManager = FbxManager::Create();
	if (!SdkManager) {
		std::cerr << "Error: Unable to create FBX Manager!\n";
		return false;
	}

	// Create the IO settings object
	ios = FbxIOSettings::Create(SdkManager, IOSROOT);
	SdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager
	lImporter = FbxImporter::Create(SdkManager, "");

	// Use the first argument as the filename for the importer
	if (!lImporter->Initialize(filename, -1, SdkManager->GetIOSettings())) {
		std::cerr << "Call to FbxImporter::Initialize() failed.\n";
		std::cerr << "Error returned: " << lImporter->GetStatus().GetErrorString() << "\n";
		return false;
	}

	// Create a new scene so that it can be populated by the imported file
	FbxScene* lScene = FbxScene::Create(SdkManager, "myScene");

	// Import the contents of the file into the scene
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer
	lImporter->Destroy();

	// Process the scene and extract the meshes
	ProcessNode(lScene->GetRootNode());

	// Destroy the SDK manager and all other objects it was handling
	SdkManager->Destroy();

	return true;
}

void FBXLoader::ProcessNode(FbxNode* pNode) {
	if (pNode) {
		// Process the node's attributes
		for (int i = 0; i < pNode->GetNodeAttributeCount(); i++) {
			FbxNodeAttribute* attribute = pNode->GetNodeAttributeByIndex(i);
			if (attribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
				FbxMesh* pMesh = pNode->GetMesh();
				if (pMesh) {
					// Get the vertex positions
					FbxVector4* vertices = pMesh->GetControlPoints();
					if (vertices != nullptr)
					{
						for (int i = 0; i < pMesh->GetControlPointsCount(); ++i) {
							FbxVector4 vertex = vertices[i];
							// Set FBX Vertex data to mesh
							glm::vec3 position(vertex[0], vertex[1], vertex[2]);
							vert.push_back({ position, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec2(0.0f) }); // normal, color, and texUV are initialized to zeros
						}
					}
					else
					{
						std::cout << "Error no vertices on fbx file" << std::endl;
					}

					// Get the indices
					int polygonCount = pMesh->GetPolygonCount();
					for (int i = 0; i < polygonCount; ++i) {
						int polygonSize = pMesh->GetPolygonSize(i);
						for (int j = 0; j < polygonSize; ++j) {
							int index = pMesh->GetPolygonVertex(i, j);
							// Set FBX Indeces data to mesh
							indi.push_back(index);
						}
					}
				}
			}
		}

		// Recursively process the children nodes
		for (int j = 0; j < pNode->GetChildCount(); j++) {
			ProcessNode(pNode->GetChild(j));
		}
	}
}