#include "Sprite2D.h"

Sprite2D::Sprite2D(const char* filename, glm::uint NumSpritesX, glm::uint NumSpritesY, glm::uint WindowWidth, glm::uint WindowHeight)
{
	m_filename = filename;
	m_numSpriteX = (float)NumSpritesX;
	m_numSpriteY = (float)NumSpritesY;
	m_windowWidth = (float)WindowWidth;
	m_windowHeight = (float)WindowHeight;
	m_windowAR = m_windowWidth / m_windowHeight;

	glm::uint NumSprites = NumSpritesX * NumSpritesY;

	initSpriteSheet();
	CalcSpriteInfo();
}

void Sprite2D::render(const vector<SpriteInfo>& sprites)
{
}

void Sprite2D::initSpriteSheet()
{
	//m_SpriteSheet = new Texture(m_filename, "png", GL_TEXTURE);
}

void Sprite2D::CalcSpriteInfo()
{
	int ImageWidth = 0;
	int ImageHeight = 0;

	//m_SpriteSheet->GetImageSize();

	float SpriteWidth = (float)ImageWidth / m_numSpriteX;
	float SpriteHeight = (float)ImageHeight / m_numSpriteY;

	m_spriteAspectRatio = SpriteHeight / SpriteWidth;

	m_textUSize = 1.0f / m_numSpriteX;
	m_textUSize = 1.0f / m_numSpriteY;

	m_ndcPixelX = 2.0f / m_windowWidth;
	m_ndcPixelY = 2.0f / m_windowHeight;

	m_tileHeightInPixels = 0.0f;
	m_tileWidthInPixels = 0.0f;

	float ImageWidthToWidnowWidthRatio = ImageWidth / m_windowWidth;
	float ImageHeightToWidnowHeightRatio = ImageHeight / m_windowHeight;

	if (ImageWidthToWidnowWidthRatio < ImageHeightToWidnowHeightRatio)
	{
		m_tileWidthInPixels = m_windowWidth / m_numSpriteX;
		m_tileHeightInPixels = m_windowHeight / m_numSpriteY;
	}
	else
	{
		m_tileWidthInPixels = m_windowWidth / m_numSpriteX;
		m_tileHeightInPixels = m_tileWidthInPixels * m_spriteAspectRatio;
	}

	m_titleWidthNDC = m_ndcPixelX * m_tileWidthInPixels;
	m_titleHeightNDC = m_ndcPixelY * m_tileHeightInPixels;
}
