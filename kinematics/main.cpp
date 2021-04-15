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
MaterialPoint* controlledObject;
std::vector<MaterialPoint> objects;
void createObject();
void printObjectList();
void doObjectMovement();

void printControlledObjectCharachteristics();
bool printControlledObjectData = false;

// Environment coefficient resistance 
float ambientDensity = 0.0f;



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

	// Shader
	Shader mainShader("main.vertexShader", "main.fragmentShader");

	// Coordinate System
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		doCameraMovement();
		doObjectMovement();



		if (printControlledObjectData)
		printControlledObjectCharachteristics();



		mainShader.Use();
		mainShader.setMatrix4("model", glm::mat4(1.0f));
		mainShader.setMatrix4("view", mainCamera.GetViewMatrix());
		mainShader.setMatrix4("projection", glm::perspective(mainCamera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000000.0f));

		XYZ.draw(mainShader);

		for (int i = 0; i < objects.size(); ++i)
		{
			objects[i].drawTrajectory(mainShader);
			objects[i].drawDevelopedForceVector(mainShader);
			objects[i].drawIncidentFlowForceVector(mainShader);
		}

		if (renderingDeltaTime >= 0.01f)
		{
			for (int i = 0; i < objects.size(); ++i)
				objects[i].computeInstantCharachteristics(ambientDensity, renderingDeltaTime);

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
		(*controlledObject).ProcessKeyboardObject(RAISE_DEVELOPED_FORCE_VECTOR, deltaTime);
	if (keys[GLFW_KEY_DOWN])
		(*controlledObject).ProcessKeyboardObject(LOWER_DEVELOPED_FORCE_VECTOR, deltaTime);
	if (keys[GLFW_KEY_LEFT])
		(*controlledObject).ProcessKeyboardObject(TURN_LEFT_DEVELOPED_FORCE_VECTOR, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
		(*controlledObject).ProcessKeyboardObject(TURN_RIGHT_DEVELOPED_FORCE_VECTOR, deltaTime);
	if (keys[GLFW_KEY_E])
		(*controlledObject).ProcessKeyboardObject(INCREASE_DEVELOPED_FORCE_VECTOR, deltaTime);
	if (keys[GLFW_KEY_Q])
		(*controlledObject).ProcessKeyboardObject(DECREASE_DEVELOPED_FORCE_VECTOR, deltaTime);
}

void doCameraMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		mainCamera.ProcessKeyboard(FORWARD_CAMERA, deltaTime);
	if (keys[GLFW_KEY_S])
		mainCamera.ProcessKeyboard(BACKWARD_CAMERA, deltaTime);
	if (keys[GLFW_KEY_A])
		mainCamera.ProcessKeyboard(LEFT_CAMERA, deltaTime);
	if (keys[GLFW_KEY_D])
		mainCamera.ProcessKeyboard(RIGHT_CAMERA, deltaTime);

	if (keys[GLFW_KEY_SPACE])
		mainCamera.ProcessKeyboard(RAISE_CAMERA, deltaTime);
	if (keys[GLFW_KEY_LEFT_SHIFT])
		mainCamera.ProcessKeyboard(LOWER_CAMERA, deltaTime);

	if (keys[GLFW_KEY_LEFT_BRACKET])
		mainCamera.ProcessKeyboard(INCREASE_CAMERA_VELOCITY, deltaTime);
	if (keys[GLFW_KEY_RIGHT_BRACKET])
		mainCamera.ProcessKeyboard(DECREASE_CAMERA_VELOCITY, deltaTime);

	if (keys[GLFW_KEY_X] && keys[GLFW_KEY_0])
		mainCamera.SetCameraPoisition(glm::vec3(5.0f, 5.0f, 5.0f), 45.0f, 45.0f);

	if (keys[GLFW_KEY_X] && keys[GLFW_KEY_O])
	{
		glm::vec3 coordinates = (*controlledObject).getObjectCoordinates();
		coordinates.x += 20.0f;
		coordinates.y += 20.0f;

		const float yaw = (*controlledObject).getObjectTheta();
		const float pitch = (*controlledObject).getObjectPh();

		mainCamera.SetCameraPoisition(coordinates, yaw, pitch);
	}
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

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		std::cin >> ambientDensity;

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