#include <iostream>
#include "MaterialPoint.h"

//GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>

//GLFW
#include <GLFW/glfw3.h>


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


const GLuint WIDTH = 1366, HEIGHT = 768;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Kinematics", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);





		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}

	/*MaterialPoint object = createMaterialPoint();

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

std::cout << object.distance;*/