#include "PNGTuber.h"


PNGTuber::PNGTuber(const char* path, float width, float height, float posX, float posY, Shader& shader) : m_surface(path, width, height, posX, posY, shader)
{
	
}

// Effects and tracking
void PNGTuber::update()
{
}

void PNGTuber::render()
{
	m_surface.Bind();
}
