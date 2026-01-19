#pragma once
#include <glm/glm.hpp>
#include <vector>


struct Sphere {

	
	glm::vec3 position{ 0.0f };
	glm::vec3 albedo{ 1.0f };
	float radius;
};


struct  Scene
{
	std::vector<Sphere> spheres;
};