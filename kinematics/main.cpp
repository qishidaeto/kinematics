// STD INCLUDES
#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/vec3.hpp>

// MATH
#define _USE_MATH_DEFINES
#include <math.h>

// CLASSES
#include "MaterialPoint.h"
#include "Shader.h"
#include "Camera.h"

const GLuint WIDTH = 1366, HEIGHT = 768;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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

	Shader axesShader("axes.vs", "axes.frag");

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