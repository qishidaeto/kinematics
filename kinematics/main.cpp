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
const GLuint screenWidth = 1366, screenHeight = 768;

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
MaterialPoint* controlledObject;
std::vector<MaterialPoint> objects;
void createObject();
void printObjectList();
void doObjectMovement();

void printControlledObjectCharachteristics();
bool printControlledObjectData = false;

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
		doObjectMovement();

		if (printControlledObjectData)
		printControlledObjectCharachteristics();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		XYZ.draw(mainCamera, screenWidth, screenHeight);

		for (int i = 0; i < objects.size(); ++i)
		{
			objects[i].drawObjectTrajectory(mainCamera, screenWidth, screenHeight);
			objects[i].drawObjectForceVector(mainCamera, screenWidth, screenHeight);
		}

		if (renderingDeltaTime >= 0.01f)
		{
			for (int i = 0; i < objects.size(); ++i)
				objects[i].computeInstantCharachteristics(renderingDeltaTime);

			renderingDeltaTime = 0.0f;
		}

		if (renderingDeltaTime >= 0.01f)
			renderingDeltaTime = 0.0f;

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void printControlledObjectCharachteristics()
{
		(*controlledObject).printObjectCharachteristics();
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

	std::cout << "Enter the starting position of the object(x, y, z): ";
	float x, y, z;
	std::cin >> x >> y >> z;
	glm::vec3 coordinates = { x, y, z };

	MaterialPoint object(name, mass, coordinates);
	objects.push_back(object);

	controlledObject = &objects[objects.size() - 1];

	system("cls");
}

void printObjectList()
{
	system("cls");

	int i = 1;

	for (const MaterialPoint& object : objects)
		std::cout << i++ << '.' << object.getObjectName() << std::endl;
}

void selectControlledObject()
{
	printObjectList();

		int objectNumber = 0;
		std::cin >> objectNumber;

		if (objectNumber > 0 && objectNumber <= objects.size())
			controlledObject = &objects[objectNumber - 1];

		system("cls");
}

void doObjectMovement()
{
	// Object controls
	if (keys[GLFW_KEY_UP])
		(*controlledObject).ProcessKeyboardObject(UP_OBJECT, deltaTime);
	if (keys[GLFW_KEY_DOWN])
		(*controlledObject).ProcessKeyboardObject(DOWN_OBJECT, deltaTime);
	if (keys[GLFW_KEY_LEFT])
		(*controlledObject).ProcessKeyboardObject(TURN_LEFT_OBECT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
		(*controlledObject).ProcessKeyboardObject(TURN_RIGHT_OBJECT, deltaTime);
	if (keys[GLFW_KEY_E])
		(*controlledObject).ProcessKeyboardObject(INCREASE_FORCE_OBJECT, deltaTime);
	if (keys[GLFW_KEY_Q])
		(*controlledObject).ProcessKeyboardObject(DECREASE_FORCE_OBJECT, deltaTime);
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

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		selectControlledObject();

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		if (printControlledObjectData)
			printControlledObjectData = false;
		else printControlledObjectData = true;

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