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

	static uint32_t PCG_Hash(uint32_t input) {
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}
}


void Renderer::OnResize(uint32_t width, uint32_t height) {


	if (finalImage) {

		// There was no need for any resizes
		if (finalImage -> GetHeight() == height && finalImage -> GetWidth() == width) 
			return;

		finalImage->Resize(width, height);
		
	}
	else {
		finalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] accumulationData;
	accumulationData = new glm::vec4[width * height];

	imageHorizontalIterator.resize(width);
	imageVerticalIterator.resize(height);

	for (uint32_t i = 0; i < width; i++)
	{
		imageHorizontalIterator[i] = i;
	}
	for (uint32_t i = 0; i < height; i++)
	{
		imageVerticalIterator[i] = i;
	}
}

void Renderer::Render(const Scene& scene, const Camera& camera) {
	//std::for_each()
	activeCamera = &camera;
	activeScene = &scene;
	const glm::vec3& rayOrigin = camera.GetPosition(); 
	if (frameIndex == 1)
		memset(accumulationData, 0, finalImage->GetHeight() * finalImage->GetWidth() * sizeof(glm::vec4));

#define M_T 1
#if M_T 1
	std::for_each(std::execution::par, imageVerticalIterator.begin(), imageVerticalIterator.end(),
		[this](uint32_t y) {
			std::for_each(std::execution::par, imageHorizontalIterator.begin(), imageHorizontalIterator.end(),
				[this, y](uint32_t x) {
					glm::vec4 color = RayGeneration(x, y);

					accumulationData[x + y * finalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = accumulationData[x + y * finalImage->GetWidth()];
					accumulatedColor /= (float)frameIndex;
					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

					m_ImageData[x + finalImage->GetWidth() * y] = Utilities::ConvertToRGBA(accumulatedColor);
				});
		});
#else
	for (uint32_t y = 0; y < finalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < finalImage->GetWidth(); x++) {

			glm::vec4 color = RayGeneration(x, y);

			accumulationData[x + y * finalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = accumulationData[x + y * finalImage->GetWidth()];
			accumulatedColor /= (float)frameIndex;
			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + finalImage->GetWidth() * y] = Utilities::ConvertToRGBA(accumulatedColor);

		}
	}
#endif

	finalImage->SetData(m_ImageData);

	if (settings.Accumulate) frameIndex++;
	else frameIndex = 1;
	 

}
glm::vec4 Renderer::RayGeneration(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = activeCamera->GetPosition();
	ray.Direction = activeCamera->GetRayDirections()[x + y * finalImage->GetWidth()];
	
	glm::vec3 finalColor(0.0f);

	int bounces =5; 
	glm::vec3 contributions = glm::vec3(0.8f,1.0f,0.8f);
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload =  TraceRay(ray); 
		if (payload.hitTValue < 0.0f) {
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			//finalColor += skyColor * contributions;
			break;
		}
		
		const Sphere& sphere = activeScene->spheres[payload.ObjectIndex];
		const Material& material = activeScene->materials[sphere.materialIndex];
		glm::vec3 lighting(0.0f);
		for (const auto& light : activeScene->lights)
		{
			if (light.lightColor == glm::vec3(0.0f)) continue;
			glm::vec3 lightDir = glm::normalize(-light.lightDirection);
			float NdotL = glm::max(glm::dot(payload.worldNormal, lightDir), 0.0f);
			lighting +=  material.albedo * light.lightColor * light.intensity * NdotL;
		}
		//finalColor += lighting * contributions;
		contributions  *= material.albedo;
		finalColor += material.GetEmission() * contributions ;
		ray.Origin = payload.worldPos + 0.001f * payload.worldNormal;
		/*ray.Direction = glm::normalize(glm::reflect(ray.Direction,
			payload.worldNormal + material.roughness * 0.5f*Walnut::Random::InUnitSphere()));*/
		ray.Direction = glm::normalize(payload.worldNormal + Walnut::Random::InUnitSphere());

	
	}
	return glm::vec4(finalColor, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	


	int closestSphere = -1;
	float hitTval = FLT_MAX;

	for (int i=0; i< activeScene->spheres.size(); i++ )
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

		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (0.0f < closestT && closestT < hitTval) {
			hitTval = closestT; 
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

	
	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.hitTValue = -1.0f;
	return payload;
}



