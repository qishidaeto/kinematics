// Std. Includes
#include <iostream>
#include <vector>

//ImGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm/vec3.hpp>
#include <glm/glm/mat4x4.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

// Std. Math
#define _USE_MATH_DEFINES
#include <math.h>

// Classes
#include "Shader.h"
#include "Camera.h"
#include "CoordinateSystem.h"
#include "MaterialPoint.h"


// Object forms
#define FLAT_PLATE_FORM                          1
#define SPHERE_FORM                              2
#define ROUNDED_HEMISHPERE_FORM                  3
#define FLAT_HEMISPHERE_FORM                     4
#define HORIZONTAL_ELLIPSOID_FORM                5
#define CUBE_FORM                                6

// Drag coefficients
#define FLAT_PLATE_DRAG_COEFFICIENT              1.17f
#define SPHERE_DRAG_COEFFICIENT                  0.47f
#define ROUNDED_HEMISHPERE_DRAG_COEFFICIENT      0.42f
#define FLAT_HEMISPHERE_DRAG_COEFFICIENT         1.17f
#define VERTICAL_ELLIPSOID_DRAG_COEFFICIENT      0.59f
#define CUBE_DRAG_COEFFICIENT                    2.05f

// Callback-functions
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void voidCallBackFunction(GLFWwindow* window, double xpos, double ypos) {}

// Screen resolution
const GLuint screenWidth = 1920, screenHeight = 1080;

// Camera
Camera mainCamera(glm::vec3(10.0f, 10.0f, 10.0f));
void doCameraMovement();

// Variables for controlling
bool keys[1024];
GLfloat lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool showCursor = false;

// Objects
MaterialPoint* controlledObject = nullptr;
std::vector<MaterialPoint> objects;
void doObjectMovement();

// Ambient density (for calculation of environment coefficient resistance)
float ambientDensity = 0.0f;

// GUI Menu
bool menuCreateObject = false;
bool menuObjectList = false;
void displayGUImenu();

int WinMain()
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	// Shader
	Shader mainShader("main.vertexShader", "main.fragmentShader");

	// 3D Coordinate system
	CoordinateSystem XYZ;

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0f);
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

		displayGUImenu();

		mainShader.Use();
		mainShader.setMatrix4("model", glm::mat4(1.0f));
		mainShader.setMatrix4("view", mainCamera.GetViewMatrix());
		mainShader.setMatrix4("projection", glm::perspective(mainCamera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000000.0f));

		XYZ.draw(mainShader);

		for (int i = 0; i < objects.size(); ++i)
		{
			if (objects[i].getDrawStatus(TRAJECTORY))
			objects[i].drawTrajectory(mainShader);

			if (objects[i].getDrawStatus(DEVELOPED_FORCE))
			objects[i].drawDevelopedForceVector(mainShader);

			if (objects[i].getDrawStatus(DRAG_FORCE))
			objects[i].drawDragForceVector(mainShader);

			if (objects[i].getDrawStatus(GRAVITATIONAL_FORCE))
			objects[i].drawGravitationalForceVector(mainShader);
		}

		if (renderingDeltaTime >= 0.10f)
			renderingDeltaTime = 0.0f;

		if (renderingDeltaTime >= 0.01f)
		{
			for (int i = 0; i < objects.size(); ++i)
				objects[i].computeInstantCharachteristics(objects, ambientDensity, renderingDeltaTime);

			renderingDeltaTime = 0.0f;
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void displayGUImenu()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Create Object"))
				menuCreateObject = true;

			if (ImGui::MenuItem("Objects List"))
				menuObjectList = true;

			ImGui::EndMenu();
		}

		if (menuCreateObject)
		{
			ImGui::SetNextWindowPos({ screenWidth - 315.0f, 30.0f });
			ImGui::SetNextWindowSize({ 250.0f, 400.0f });

			ImGui::Begin("Object creating", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			static char nameBuffer[16];
			ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_None | ImGuiInputTextFlags_CharsNoBlank);

			static float mass = 0.1f;
			ImGui::InputFloat("Mass", &mass, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsScientific);

			if (mass <= 0.0f)
			{
				ImGui::PushStyleColor(NULL, { 1.0f, 0.0f, 0.0f, 1.0f });
				ImGui::Text("Incorrect object mass");
				ImGui::PopStyleColor();
			}

			static GLfloat x = 0.0f;
			ImGui::InputFloat("X", &x, 0.1f, 0.1f, "%.3f");

			static GLfloat y = 0.0f;
			ImGui::InputFloat("Y", &y, 0.1f, 0.1f, "%.3f");

			static GLfloat z = 0.0f;
			ImGui::InputFloat("Z", &z, 0.1f, 0.1f, "%.3f");

			ImGui::Text("Object form (Cf):");
			static int objectForm = 0;

			ImGui::RadioButton("Flat plate", &objectForm, FLAT_PLATE_FORM);
			ImGui::RadioButton("Sphere", &objectForm, SPHERE_FORM);
			ImGui::RadioButton("Rounded hemisphere", &objectForm, ROUNDED_HEMISHPERE_FORM);
			ImGui::RadioButton("Flat hemisphere", &objectForm, FLAT_HEMISPHERE_FORM);
			ImGui::RadioButton("Horizontal ellipsoid", &objectForm, HORIZONTAL_ELLIPSOID_FORM);
			ImGui::RadioButton("Cube", &objectForm, CUBE_FORM);

			ImGui::Text("Midsection:");

			static GLfloat midsection = 0.0f;
			ImGui::InputFloat("S", &midsection, 0.1f, 0.1f, "%.3f");

			if (ImGui::Button("Create"))
			{
				float dragCoefficient = 0.0f;

				if (objectForm == FLAT_PLATE_FORM) dragCoefficient = FLAT_PLATE_DRAG_COEFFICIENT;
				if (objectForm == SPHERE_FORM) dragCoefficient = SPHERE_DRAG_COEFFICIENT;
				if (objectForm == ROUNDED_HEMISHPERE_FORM) dragCoefficient = ROUNDED_HEMISHPERE_DRAG_COEFFICIENT;
				if (objectForm == FLAT_HEMISPHERE_FORM) dragCoefficient = FLAT_HEMISPHERE_DRAG_COEFFICIENT;
				if (objectForm == HORIZONTAL_ELLIPSOID_FORM) dragCoefficient = VERTICAL_ELLIPSOID_DRAG_COEFFICIENT;
				if (objectForm == CUBE_FORM) dragCoefficient = CUBE_DRAG_COEFFICIENT;

				objects.push_back({ nameBuffer, mass, {x, y, z}, dragCoefficient, midsection });
				controlledObject = &objects[objects.size() - 1];
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
				menuCreateObject = false;

			ImGui::End();
		}

		if (menuObjectList)
		{
			ImGui::SetNextWindowPos({ 315.0f, 30.0f });
			ImGui::SetNextWindowSize({ 275.0f, 800.0f });

			ImGui::Begin("Objects list", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			if (ImGui::TreeNode("Objects"))
			{
				for (int i = 0; i < objects.size(); ++i)
				{
					if (i == 0)
						ImGui::SetNextItemOpen(true, ImGuiCond_Once);

					if (ImGui::TreeNode((void*)(intptr_t)i, "%s", objects[i].getObjectName()))
					{
						ImGui::Text("Coordinates:");
						ImGui::Text("X:%f m", objects[i].getObjectCoordinates().x);
						ImGui::Text("Y:%f m", objects[i].getObjectCoordinates().y);
						ImGui::Text("Z:%f m", objects[i].getObjectCoordinates().z);

						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

						ImGui::Text("Developed Force(F):");
						ImGui::Text("Fx:%f N", objects[i].getObjectDevelopedForceVector().x);
						ImGui::Text("Fy:%f N", objects[i].getObjectDevelopedForceVector().y);
						ImGui::Text("Fz:%f N", objects[i].getObjectDevelopedForceVector().z);

						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

						ImGui::Text("Drag Force(Fd):");
						ImGui::Text("Fdx:%f N", objects[i].getObjectDragForceVector().x);
						ImGui::Text("Fdy:%f N", objects[i].getObjectDragForceVector().y);
						ImGui::Text("Fdz:%f N", objects[i].getObjectDragForceVector().z);

						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

						ImGui::Text("Gravitational Force(Fg):");
						ImGui::Text("Fgx:%f N", objects[i].getObjectGravitationalForceVector().x);
						ImGui::Text("Fgy:%f N", objects[i].getObjectGravitationalForceVector().y);
						ImGui::Text("Fgz:%f N", objects[i].getObjectGravitationalForceVector().z);

						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

						if (ImGui::SmallButton("Delete object")) 
							objects.erase(objects.begin() + i);

						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			ImGui::End();
		}

		ImGui::EndMainMenuBar();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Object controls
void doObjectMovement()
{
	if (controlledObject != nullptr)
	{
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
}

// Camera controls
void doCameraMovement()
{
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
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		if (!showCursor)
		{
			showCursor = true;
			glfwSetCursorPosCallback(window, voidCallBackFunction);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		else
		{
			showCursor = false;
			glfwSetCursorPosCallback(window, mouseCallback);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
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