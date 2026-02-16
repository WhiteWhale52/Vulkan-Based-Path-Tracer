#pragma once
#include <glm/glm.hpp>
#include <vector>


struct Material {
	glm::vec3 albedo{ 1.0f };
	float roughness = 0.0f;
	float metallic = 0.0f;
	float emissionPower = 0.4f;
	glm::vec3 emissionColor{ 0.0f };

	glm::vec3 GetEmission() const { return emissionColor * emissionPower; }
};

struct Sphere {

	
	glm::vec3 position{ 0.0f };
	int materialIndex = 0;
	float radius = 0.1f;
};

struct Light {
	glm::vec3 lightColor{ 1.0f };
	glm::vec3 lightDirection{ 1.0f };
	float intensity = 0.3f;
};


struct  Scene
{
	std::vector<Sphere> spheres;
	std::vector<Light> lights;
	std::vector<Material> materials;
};