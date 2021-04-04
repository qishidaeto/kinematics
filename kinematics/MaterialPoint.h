#pragma once

//GLM
#include <glm/vec3.hpp>

#include <string>
#include <math.h>


class MaterialPoint
{
public:
	MaterialPoint(
		std::string new_name,
		const float& new_mass,
		const glm::vec3& new_force,
		const glm::vec3& new_coordinates
	)
	{
		name = new_name;
		mass = new_mass;
		force = new_force;
		coordinates = new_coordinates;
		
		velocity.x = 0.0f;
		velocity.y = 0.0f;
		velocity.z = 0.0f;

		distance = 0;
	}

	void computeInstantCharachteristics()
	{
		acceleration.x = force.x / mass;
		acceleration.y = force.y / mass;
		acceleration.z = force.z / mass;

		velocity.x += acceleration.x;
		velocity.y += acceleration.y;
		velocity.z += acceleration.z;

		float x0 = coordinates.x;
		float y0 = coordinates.y;
		float z0 = coordinates.z;

		coordinates.x += velocity.x;
		coordinates.y += velocity.y;
		coordinates.z += velocity.z;

		distance += sqrt(pow(coordinates.x - x0, 2) + pow(coordinates.y - y0, 2) + pow(coordinates.z - z0, 2));
	}

	void updateCoordinateBuffer()
	{

	}

	void drawTrajectory()
	{

	}

private:
	std::string name;

	float mass;
	float distance;

	glm::vec3 force;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec3 coordinates;


};