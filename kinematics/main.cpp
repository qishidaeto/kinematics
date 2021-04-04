// Std. Includes
#include <iostream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/vec3.hpp>

// Std. Math
#define _USE_MATH_DEFINES
#include <math.h>

// Classes
#include "MaterialPoint.h"
#include "Shader.h"
#include "Camera.h"

struct coordinateSystem
{
	coordinateSystem()
	{
		GLfloat vertices[] =
		{
			 // Axis X
			-10000.f, 0.0f, 0.0f,
			 10000.f, 0.0f, 0.0f,

			 // Axis Y
			 0.0f, -10000.f, 0.0f,
			 0.0f,  10000.f, 0.0f,

			 // Axis Z
			 0.0f, 0.0f, -10000.f,
			 0.0f, 0.0f,  10000.f
		};

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	~coordinateSystem()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	GLuint VAO, VBO;
};

const GLuint screenWidth = 1920, screenHeight = 1080;

// Callback-functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Camera
Camera camera(glm::vec3(10.0f, 10.0f, 10.0f));

bool keys[1024];
GLfloat lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void Do_Movement();

// Objects
std::vector<MaterialPoint> objects;
void createObject();
void doObjectMovement();

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

	Shader shader("shader.vs", "shader.frag");
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

		glfwPollEvents();
		Do_Movement();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shader.Use();

		// Load transformation matrices into shader
		shader.setMatrix4("model", glm::mat4(1.0f));
		shader.setMatrix4("view", camera.GetViewMatrix());
		shader.setMatrix4("projection", glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.001f, 1000000.0f));

		glBindVertexArray(XYZ.VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glDrawArrays(GL_LINE_STRIP, 2, 2);
		glDrawArrays(GL_LINE_STRIP, 4, 2);
		glBindVertexArray(0);


		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void createObject()
{
	std::cout << "Enter the object name: ";
	std::string name;
	std::cin >> name;

	std::cout << "Enter the mass of the object: ";
	float mass;
	std::cin >> mass;

	std::cout << "Enter the radius-vector of the force of the object(x, y, z): ";
	float Fx, Fy, Fz;
	std::cin >> Fx >> Fy >> Fz;
	glm::vec3 force = { Fx, Fy, Fz };


	std::cout << "Enter the starting position of the object(x, y, z): ";
	float x, y, z;
	std::cin >> x >> y >> z;
	glm::vec3 coordinates = { x, y, z };

	MaterialPoint object(name, mass, force, coordinates);
	objects.push_back(object);
}

void doObjectMovement()
{
	//calculate charachteristics
	//update buffer
	//draw array
}

void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
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

	//if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	//	doObjectMovement();
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}