#include "PNGTuber.h"
#include <GLFW/glfw3.h>

// Effects
void PNGTuber::update()
{
	// Bounce
	if (m_bounceMin != glm::vec2(0.0f, 0.0f) && m_bounceMAX != glm::vec2(0.0f, 0.0f)) {
		// Apply bounce logic, here it's a simple oscillation using sine function
		float bounceSpeed = 0.1f; // This controls how fast it bounces
		float bounceFactor = glm::sin(glfwGetTime() * bounceSpeed); // Time-based oscillation

		// Map bounceFactor between m_bounceMin and m_bounceMAX
		float newX = glm::mix(m_bounceMin.x, m_bounceMAX.x, (bounceFactor + 1.0f) / 2.0f); // Map to [0, 1]
		float newY = glm::mix(m_bounceMin.y, m_bounceMAX.y, (bounceFactor + 1.0f) / 2.0f); // Map to [0, 1]

		// Apply new position based on bounce
		m_surface.SetPosition(newX, newY);
	}

	// Squish
	if (m_squishMin != glm::vec2(0.0f, 0.0f) && m_squishMAX != glm::vec2(0.0f, 0.0f)) {
		float squishSpeed = 0.2f;
		float squishFactor = glm::sin(glfwGetTime() * squishSpeed);

		float scaleX = glm::mix(m_squishMin.x, m_squishMAX.x, (squishFactor + 1.0f) / 2.0f);
		float scaleY = glm::mix(m_squishMin.y, m_squishMAX.y, (squishFactor + 1.0f) / 2.0f);

		// Apply scale, you can extend Surface to have SetScale method
		m_surface.SetScale(scaleX, scaleY);
	};

	// Freqence
	if (m_freqenceMin != glm::vec2(0.0f, 0.0f) && m_freqenceMAX != glm::vec2(0.0f, 0.0f)) {
		float freqSpeed = 0.5f;
		float frequency = glm::mix(m_freqenceMin.x, m_freqenceMAX.x, (glm::sin(glfwGetTime() * freqSpeed) + 1.0f) / 2.0f);

		// Use frequency to modulate some other transformation, like bounce speed
		float bounceFactor = glm::sin(glfwGetTime() * frequency);
		float newX = glm::mix(m_bounceMin.x, m_bounceMAX.x, (bounceFactor + 1.0f) / 2.0f);

		m_surface.SetPosition(newX, m_surface.getY());
	}

	// Amplitude
	if (m_amplitudeMin != glm::vec2(0.0f, 0.0f) && m_amplitudeMAX != glm::vec2(0.0f, 0.0f)) {
		float amplitudeFactor = glm::sin(glfwGetTime());

		// Move within amplitude limits
		float newX = glm::mix(m_amplitudeMin.x, m_amplitudeMAX.x, (amplitudeFactor + 1.0f) / 2.0f);
		float newY = glm::mix(m_amplitudeMin.y, m_amplitudeMAX.y, (amplitudeFactor + 1.0f) / 2.0f);

		m_surface.SetPosition(newX, newY);
	}

	// Rotaional
	if (m_rotaionalMin && m_rotaionalMAX != 0.0f) {
		float rotationSpeed = 0.3f;
		float rotation = glm::mix(m_rotaionalMin, m_rotaionalMAX, (glm::sin(glfwGetTime() * rotationSpeed) + 1.0f) / 2.0f);

		// Apply rotation, you might need to extend Surface to support rotation
		m_surface.SetRotation(rotation);
	}

	// Drag
	if (m_dragMin != 0.0f && m_dragMAX != 0.0f) {
		float drag = glm::mix(m_dragMin, m_dragMAX, (glm::sin(glfwGetTime()) + 1.0f) / 2.0f);

		// Apply drag to movement (e.g., reduce velocity over time)
		float newX = m_surface.getX() * drag;
		float newY = m_surface.getY() * drag;

		m_surface.SetPosition(newX, newY);
	}
}

void PNGTuber::render()
{
	m_surface.Bind();
}

void PNGTuber::Delete()
{
	m_surface.Delete();
}
