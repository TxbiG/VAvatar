#ifndef CLASS_PNGTUBER_H
#define CLASS_PNGTUBER_H

#include <vector>
#include "../Surface.h"

using namespace std;

// To do - 
// Fix Effects in update
// Start working on the editor

class PNGTuber
{
public:
	PNGTuber(const char* path, float width, float height, float posX, float posY, Shader& shader) : m_surface(path, width, height, posX, posY, shader) {};

	void update();
	void render();
	void Delete();

	// Set Axis value effects
	void setBounce(glm::vec2 min, glm::vec2 max)		{ m_bounceMin = min;	m_bounceMAX = max; }
	void setSquish(glm::vec2 min, glm::vec2 max)		{ m_squishMin = min;	m_squishMAX = max; }
	void setFreqence(glm::vec2 min, glm::vec2 max)		{ m_freqenceMin = min;	m_freqenceMAX = max; }
	void setAmplitude(glm::vec2 min, glm::vec2 max)	{ m_amplitudeMin = min; m_amplitudeMAX = max; }
	void setRotaional(float angleMin, float angleMax)	{ m_rotaionalMin = angleMin; m_rotaionalMAX = angleMax; }
	void setDrag(float min, float max)					{ m_dragMin = min;		m_dragMin = max; }
	void setColor(float r, float g, float b)			{ m_color.r = r; m_color.g = g; m_color.b = b; }

private:
	//enum Emotions { Neutral, Joy, Sadness, Anger, Fear, Disgust };

	// Effects
	// Min
	glm::vec2 m_bounceMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_squishMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_freqenceMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_amplitudeMin = glm::vec2(0.0f, 0.0f);
	float m_rotaionalMin = 0.0f;
	float m_dragMin = 0.0f;
	// Max
	glm::vec2 m_bounceMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_squishMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_freqenceMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_amplitudeMAX = glm::vec2(0.0f, 0.0f);
	float m_rotaionalMAX = 0.0f;
	float m_dragMAX = 0.0f;

	glm::vec3 m_color =  glm::vec3(0.0f, 0.0f, 0.0f);

	Surface m_surface;
};
#endif // CLASS_PNGTUBER_H