#ifndef CLASS_PNGTUBER_H
#define CLASS_PNGTUBER_H

#include <vector>
#include "../Surface.h"

using namespace std;

enum Emotions { Neutral, Joy, Sadness, Anger, Fear, Disgust };
enum Status { Talking, Silent };



// To do - 
// Get Multiple Textures
// Switch from talking and not

class PNGTuber
{
public:
	PNGTuber(const char* path, float width, float height, float posX, float posY, Shader& shader);

	void update();
	void render();

	// Set Axis value effects
	float setbounce(glm::vec2 min, glm::vec2 max)		{ m_bounceMin = min;	m_bounceMAX = max; }
	float setsquish(glm::vec2 min, glm::vec2 max)		{ m_squishMin = min;	m_squishMAX = max; }
	float setfreqence(glm::vec2 min, glm::vec2 max)		{ m_freqenceMin = min;	m_freqenceMAX = max; }
	float setamplitude(glm::vec2 min, glm::vec2 max)	{ m_amplitudeMin = min; m_amplitudeMAX = max; }
	float setrotaional(glm::vec2 min, glm::vec2 max)	{ m_rotaionalMin = min; m_rotaionalMAX = max; }
	float setdrag(glm::vec2 min, glm::vec2 max)			{ m_dragMin = min;		m_dragMin = max; }

	float setColor(glm::vec3 color)		{ m_color = color; }

private:
	bool m_isBounce = false;
	bool m_isSquish = false;
	bool m_isFreqence = false;
	bool m_isAmplitude = false;
	bool m_isRotaional = false;
	bool m_isDrag = false;

	bool m_trackAudio = false;
	//bool trackCamera = false;



	// Effects
	// Min
	glm::vec2 m_bounceMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_squishMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_freqenceMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_amplitudeMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_rotaionalMin = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_dragMin = glm::vec2(0.0f, 0.0f);
	// Max
	glm::vec2 m_bounceMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_squishMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_freqenceMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_amplitudeMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_rotaionalMAX = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_dragMAX = glm::vec2(0.0f, 0.0f);

	glm::vec3 m_color =  glm::vec3(0.0f, 0.0f);

	Surface m_surface;
};
#endif // CLASS_PNGTUBER_H