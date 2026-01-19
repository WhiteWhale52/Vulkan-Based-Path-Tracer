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
			| (b << 16)
			| (g << 8)
			| r;

	}
}


void Renderer::OnResize(uint32_t width, uint32_t height) {


	if (m_FinalImage) {

		// There was no need for any resizes
		if (m_FinalImage -> GetHeight() == height && m_FinalImage -> GetWidth() == width) 
			return;

		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera) {
	//std::for_each()
	activeCamera = &camera;
	activeScene = &scene;
	const glm::vec3& rayOrigin = camera.GetPosition(); 
	Ray ray;
	ray.Origin = camera.GetPosition();
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			RayGeneration();
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];
			glm::vec4 color = TraceRay(scene, ray);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + m_FinalImage->GetWidth() * y] = Utilities::ConvertToRGBA(color);

		}
	}


	m_FinalImage->SetData(m_ImageData);
	 

}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	glm::vec3 lightDir(-2.0f, -1.0f, -2.0f);
	lightDir = glm::normalize(lightDir);

	if (scene.spheres.size() == 0) return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitTval = FLT_MAX;
	for (const Sphere& sphere : scene.spheres)
	{
		glm::vec3 origin = ray.Origin - sphere.position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f) {
			continue;
		}

		float t_0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (t_0 < hitTval) {
			hitTval = t_0; 
			closestSphere = &sphere;
		}//glm::vec3 sphereColor(1.0f, 0.0f, 1.0f);
	}

	if (closestSphere == nullptr) return glm::vec4(0.0f,0.0f,0.0f,1.0f);

	glm::vec3 origin = ray.Origin - closestSphere->position; 

	glm::vec3 hitPos = hitTval * ray.Direction + origin;
	glm::vec3 normal = glm::normalize(hitPos);

	float lightIntensity = glm::max(glm::dot(-lightDir, glm::normalize(normal)), 0.0f);
	glm::vec3 sphereColor = closestSphere->albedo;
	sphereColor *= lightIntensity;
	return glm::vec4(sphereColor, 1.0f);

}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitTVal, uint32_t objectIndex)
{
	return HitPayload();
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	return HitPayload();
}

glm::vec4 Renderer::RayGeneration()
{
	return glm::vec4();
}

