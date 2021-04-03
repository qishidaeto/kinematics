#include <iostream>
#include "MaterialPoint.h"

//GLM
#include <glm/vec3.hpp>

//MATH
#define _USE_MATH_DEFINES
#include <math.h>

//d2r - degrees to radians
double d2r(const float& angle) { return angle * M_PI / 180.0; }

MaterialPoint createMaterialPoint()
{
	std::cout << "mass: ";
	float mass;
	std::cin >> mass;

	std::cout << "force and it's direction(angle): ";
	float force_absValue;
	float force_angleToHorizon;
	std::cin >> force_absValue >> force_angleToHorizon;

	glm::vec3 force = {
		force_absValue * cos(d2r(force_angleToHorizon)),
		force_absValue * sin(d2r(force_angleToHorizon)),
		0.0f };

	std::cout << "coordinates: ";
	float x;
	float y;
	std::cin >> x >> y;
	glm::vec3 coordinates = { x, y, 0.0f };

	MaterialPoint object(mass, force, coordinates);
	return object;
}

int main()
{
	MaterialPoint object = createMaterialPoint();

	float elapsedTime = 0.0f;

	std::cout << "time: ";
	float time;
	std::cin >> time;

	float currentTime = elapsedTime;
	float dt = 0.2f;

	while (currentTime <= elapsedTime + time)
	{
		object.velocity.x = object.acceleration.x * dt;
		object.velocity.y = object.acceleration.y * dt;

		object.coordinates.x += object.velocity.x * dt;
		object.coordinates.y += object.velocity.y * dt;

		object.distance += sqrt(pow(object.velocity.x, 2) + pow(object.velocity.y, 2)) * dt;

		currentTime += dt;
	}

	elapsedTime += time;

	std::cout << object.distance;

	return 0;
}