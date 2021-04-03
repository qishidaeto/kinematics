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
	std::cin >> force_absValue;

	float force_angleToHorizon;
	std::cin >> force_angleToHorizon;

	glm::vec3 force = { force_absValue * cos(d2r(force_angleToHorizon)), force_absValue * sin(d2r(force_angleToHorizon)), 0.0f };

		MaterialPoint object(mass, force);
		return object;
}

int main()
{
	MaterialPoint object = createMaterialPoint();

	return 0;
}