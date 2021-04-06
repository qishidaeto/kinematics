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
#include "MaterialPoint.h"

// Callback-functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const GLuint screenWidth = 1920, screenHeight = 1080;

// Camera
Camera mainCamera;

bool keys[1024];
GLfloat lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void Do_Movement();

// Objects
MaterialPoint* controlledObject;
std::vector<MaterialPoint> objects;
void createObject();
void printObjectList();
void selectObject();

//Rendering
GLfloat renderingDeltaTime = 0;

struct coordinateSystem
{
	coordinateSystem()
	{
		GLfloat vertices[] =
		{
			// Axis X
		   -10000.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			10000.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

			// Axis Y
			0.0f, -10000.f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f,  10000.f, 0.0f, 0.0f, 1.0f, 0.0f,

			// Axis Z
			0.0f, 0.0f, -10000.f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f,  10000.f, 0.0f, 0.0f, 1.0f
		};

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw(Camera& camera, const GLuint& screenWidth, const GLuint& screenHeight)
	{
		Shader axesShader("axes.vertexShader", "axes.fragmentShader");

		axesShader.Use();
		axesShader.setMatrix4("model", glm::mat4(1.0f));
		axesShader.setMatrix4("view", camera.GetViewMatrix());
		axesShader.setMatrix4("projection", glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glDrawArrays(GL_LINE_STRIP, 2, 2);
		glDrawArrays(GL_LINE_STRIP, 4, 2);
		glBindVertexArray(0);
	}

	~coordinateSystem()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	GLuint VAO, VBO;
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Kinematics", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Hide the cursor and hold it in the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);



	coordinateSystem XYZ;


	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		renderingDeltaTime += deltaTime;

		glfwPollEvents();
		Do_Movement();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		XYZ.draw(mainCamera, screenWidth, screenHeight);

		for (int i = 0; i < objects.size(); ++i)
			objects[i].drawTrajectory(mainCamera, screenWidth, screenHeight);

		if (renderingDeltaTime >= 0.01)
		{
			for (int i = 0; i < objects.size(); ++i)
				objects[i].computeInstantCharachteristics(renderingDeltaTime);

			renderingDeltaTime = 0;
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void printObjectList()
{
	system("cls");

	for (int i = 0; i < objects.size(); ++i)
		std::cout << i + 1 << ". " << objects[i].getObjectName() << std::endl;
}

void selectObject()
{
	system("cls");

	printObjectList();
	std::cout << "Select object you want to control: ";
	int object_num;
	std::cin >> object_num;

	controlledObject = &objects[object_num - 1];
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

void Do_Movement()
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		createObject();

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		printObjectList();

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		selectObject();

	if (key == GLFW_KEY_B && action == GLFW_RELEASE)
	{
		objects[0].printObjectCoordinates();
	}

	if (key == GLFW_KEY_R && action == GLFW_RELEASE)
	{
		system("cls");
		std::cout << renderingDeltaTime << std::endl;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera.ProcessMouseScroll(yoffset);
}