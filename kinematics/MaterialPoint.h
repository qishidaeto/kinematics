#pragma once

//GLM
#include <glm/vec3.hpp>

class MaterialPoint
{
public:
	MaterialPoint(
		const float& new_mass,
		const glm::vec3& new_force,
		const glm::vec3& new_coordinates
	)
	{
		mass = new_mass;
		force = new_force;
		coordinates = new_coordinates;

		acceleration.x = force.x / mass;
		acceleration.y = force.y / mass;
		acceleration.z = force.z / mass;
		
		velocity.x = 0.0f;
		velocity.y = 0.0f;
		velocity.z = 0.0f;

		distance = 0;
	}


	float mass;
	glm::vec3 force;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 coordinates;
	float distance;

	float elapsedTime;
};