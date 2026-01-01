#include "Renderer.h"


namespace Utilities {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint32_t outColor = 0x00000000;
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);
		return outColor
			| (a << 24)
			| (r << 16)
			| (g << 8)
			| b;

	}
}


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
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + m_FinalImage->GetWidth() * y] = Utilities::ConvertToRGBA(color);
		}
	}


	m_FinalImage->SetData(m_ImageData);
	 

}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDir(coord.x, coord.y, -1.0f);

	//glm::vec3 sphereColor(1.0f, 0.0f, 1.0f);

	glm::vec3 lightDir(-2.0f, -1.0f, -2.0f);
	lightDir = glm::normalize(lightDir);

	float radius = 0.5f;

	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);



	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayOrigin, rayDir);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f) {
		/*{
			float t_0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			glm::vec3 hitPos = t_0 * rayDir + rayOrigin;
			glm::vec3 normal = hitPos - glm::vec3(0.0f);

			return glm::vec4(sphereColor, 1) * glm::dot(-lightOrigin, normal);
		}*/
			
		{
			float t_0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			glm::vec3 hitPos = t_0 * rayDir + rayOrigin;
			glm::vec3 normal = hitPos - glm::vec3(0.0f);

			float d = glm::max(glm::dot(-lightDir, glm::normalize(normal)), 0.0f);

			return glm::vec4(sphereColor, 1) * d;
		}
	}

	return glm::vec4(0,0,0,1);
}
