#include "Renderer.h"



void Renderer::OnResize(uint32_t width, uint32_t height) {

	if (m_FinalImage) {

		// There was no need for any resizes
		if (m_FinalImage -> GetHeight() == height || m_FinalImage -> GetWidth() == width) 
			return;

		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render() { 
	//std::for_each()
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { (float)x / m_FinalImage->GetWidth(), (float)y / m_FinalImage->GetHeight() };
			coord = 2.0f * coord - 1.0f;
			float aspect = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
			coord.x *= aspect;
			m_ImageData[x + m_FinalImage->GetWidth() * y] = PerPixel(coord);
		}
	}


	m_FinalImage->SetData(m_ImageData);
	 

}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayOrigin(0.0f, 0.0f, 3.0f);
	glm::vec3 rayDir(coord.x, coord.y, -1.0f);

	glm::vec3 light(2.0f, 2.0f, 0.0f);

	float radius = 0.5f;

	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);



	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayOrigin, rayDir);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f) {
		return 0xff00ff00;

	}

	return 0xff000000;
}
