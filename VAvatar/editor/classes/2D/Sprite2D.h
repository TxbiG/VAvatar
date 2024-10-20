#ifndef SPRITE2D_H
#define SPRITE2D_H

#define GLM_ENABLE_EXPERIMENTAL

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <vector>
#include <string>

#include "../3D/Texture.h"
/*
	Sprite2D
*/
class Sprite2D
{
public:

	struct SpriteInfo {
		glm::uint PixelX = 0;
		glm::uint PixelY = 0;
		glm::uint SpriteColumn = 0;
		glm::uint SpriteWidth = 0;
		glm::vec3 scale = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		bool visible = true;
		float layer = 0;
	};

	Sprite2D(const char* filename, glm::uint NumSpritesX, glm::uint NumSpritesY, glm::uint WindowWidth, glm::uint WindowHeight);
	
	void render(const vector<SpriteInfo>& sprites);

private:
	void initSpriteSheet();
	void CalcSpriteInfo();

	const char* m_filename = NULL; // filepath
	float m_numSpriteX = 0.0f;
	float m_numSpriteY = 0.0f;
	float m_windowWidth = 0.0f;
	float m_windowHeight = 0.0f; // Sprite Width and Height

	// perams
	float m_spriteAspectRatio = 0.0f;
	float m_textUSize = 0.0f;
	float m_textVSize = 0.0f;
	float m_windowAR = 0.0f;
	float m_ndcPixelX = 0.0f;
	float m_ndcPixelY = 0.0f;
	float m_tileHeightInPixels = 0.0f;
	float m_tileWidthInPixels = 0.0f;
	float m_titleWidthNDC = 0.0f;
	float m_titleHeightNDC = 0.0f;
	Texture* m_SpriteSheet = NULL;
};
#endif // SPRITE2D_H
