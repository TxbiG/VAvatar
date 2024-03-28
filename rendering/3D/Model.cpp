#include"Model.h"
/*
	Read FBX file stored binary format to interpret its sturcture

	Get Mesh > Vertices, indecies

	Get Material > Get veriables for the materials / textures connected

	Get Texture > image data

	Get Skeletal rig
*/

Model::Model(const char* filename)
{
	if (!import.init(filename))
	{
		return;
	}

	vert = import.vert;
	indi = import.indi;
}

// Delete Mesh. Textures, Bones
Model::~Model()
{
}

void Model::Draw()
{
	// Draw all the meshes from the FBX file
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Mesh::Draw(matricesMeshes[i]);
	}
	
}
// Input is used to get anchors from webcam and place them on Models bones
//void Model::input() {}
// Update is anything like position, and bones
//void Model::update() {}
// Render is rending mesh (draw)
//void Model::render() {}