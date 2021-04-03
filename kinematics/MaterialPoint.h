#pragma once

#include <glm/glm.hpp>

class MaterialPoint
{
public:
	MaterialPoint(
		const float& new_mass,
		const glm::vec3 new_force
	)
	{
		mass = mass;
		force = force;
	}

private:
	float mass;
	glm::vec3 force;
};