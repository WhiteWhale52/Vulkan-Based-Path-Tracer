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
	
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec4 color = RayGeneration(x, y);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + m_FinalImage->GetWidth() * y] = Utilities::ConvertToRGBA(color);

		}
	}


	m_FinalImage->SetData(m_ImageData);
	 

}
glm::vec4 Renderer::RayGeneration(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = activeCamera->GetPosition();
	ray.Direction = activeCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	Renderer::HitPayload payload =  TraceRay(ray); 

	glm::vec3 lightDir(-2.0f, -1.0f, -2.0f);
	lightDir = glm::normalize(lightDir);
	float lightIntensity = glm::max(glm::dot(-lightDir, glm::normalize(payload.worldNormal)), 0.0f);

	const Sphere& sphere = activeScene->spheres[payload.ObjectIndex]; 

	glm::vec3 sphereColor = sphere.albedo;
	sphereColor *= lightIntensity;
	return glm::vec4(sphereColor, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	


	int closestSphere = -1;
	float hitTval = FLT_MAX;

	for ( size_t i=0; i< activeScene->spheres.size(); i++ )
	{
		const Sphere& sphere = activeScene->spheres[i];
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
			closestSphere = i;
		}
	}

	if (closestSphere < 0) 
		return Miss(ray);
	return ClosestHit(ray, hitTval, closestSphere);
	

}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitTVal, int objectIndex)
{

	Renderer::HitPayload payload;
	payload.hitTValue = hitTVal;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = activeScene->spheres[objectIndex];
	glm::vec3 origin = ray.Origin - closestSphere.position;

	payload.worldPos = hitTVal * ray.Direction + origin;
	payload.worldNormal= glm::normalize(payload.worldPos);

	payload.worldPos += closestSphere.position;

	
	return HitPayload();
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.hitTValue = -1.0f;
	return payload;
}



