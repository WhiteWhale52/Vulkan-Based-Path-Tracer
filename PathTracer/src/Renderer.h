#pragma once

#include "Walnut/Image.h"
#include <memory>
#include "Walnut/Random.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"



class Renderer {
public:
	Renderer() = default;
	
	void OnResize(uint32_t width, uint32_t height);
	
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }


private:

	struct HitPayload {
		float hitTValue;
		glm::vec3 worldNormal;
		glm::vec3 worldPos;

		uint32_t ObjectIndex;
	};

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitTVal, int objectIndex);
	HitPayload Miss(const Ray& ray);

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr; 
	glm::vec4 RayGeneration(uint32_t x, uint32_t y);

	const Scene* activeScene = nullptr;
	const Camera* activeCamera = nullptr;
};