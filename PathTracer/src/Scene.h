#pragma once
#include <glm/glm.hpp>
#include <vector>


struct Sphere {

	
	glm::vec3 position{ 0.0f };
	glm::vec3 albedo{ 1.0f };
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
};