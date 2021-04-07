// Std. Includes
#include <iostream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp>

// Std. Math
#define _USE_MATH_DEFINES
#include <math.h>

// Classes
#include "Shader.h"
#include "Camera.h"
#include "CoordinateSystem.h"
#include "MaterialPoint.h"

// Callback-functions
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

// Screen Resolution
const GLuint screenWidth = 1920, screenHeight = 1080;

// Camera
Camera mainCamera(glm::vec3(10.0f, 10.0f, 10.0f));

// Controlling
bool keys[1024];

GLfloat lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;

bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void doCameraMovement();

// Objects
std::vector<MaterialPoint> objects;
void createObject();

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Kinematics", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Hide the cursor and hold it in the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	CoordinateSystem XYZ;

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);

	//Rendering
	GLfloat renderingDeltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		renderingDeltaTime += deltaTime;

		glfwPollEvents();
		doCameraMovement();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		XYZ.draw(mainCamera, screenWidth, screenHeight);

		for (int i = 0; i < objects.size(); ++i)
			objects[i].drawObjectTrajectory(mainCamera, screenWidth, screenHeight);

		if (renderingDeltaTime >= 3.0f)
		{
			for (int i = 0; i < objects.size(); ++i)
				objects[i].computeInstantCharachteristics(renderingDeltaTime);

			renderingDeltaTime = 0.0f;
		}

		if (renderingDeltaTime >= 3.0f)
			renderingDeltaTime = 0.0f;

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}


void createObject()
{
	system("cls");

	std::cout << "Enter the object name: ";
	std::string name;
	std::cin >> name;

	std::cout << "Enter the mass of the object: ";
	float mass;
	std::cin >> mass;

	std::cout << "Enter the force abs value, theta(zenith) and ph(azimuth): ";
	float forceAbsValue, theta, ph;
	std::cin >> forceAbsValue;
	std::cin >> theta;
	std::cin >> ph;


	std::cout << "Enter the starting position of the object(x, y, z): ";
	float x, y, z;
	std::cin >> x >> y >> z;
	glm::vec3 coordinates = { x, y, z };

	MaterialPoint object(name, mass, forceAbsValue, theta, ph, coordinates);
	objects.push_back(object);

	system("cls");
}

void doCameraMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		mainCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		mainCamera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		mainCamera.ProcessKeyboard(RIGHT, deltaTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		createObject();
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	mainCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera.ProcessMouseScroll(yoffset);
}