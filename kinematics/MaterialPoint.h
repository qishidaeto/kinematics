#pragma once

//GLM
#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>

#include <fstream>
#include <iostream>
#include <string>

#define GLEW_STATIC
#include <GLEW/glew.h>

#include "Shader.h"
#include "Camera.h"


class MaterialPoint
{
public:
	MaterialPoint(
		std::string name,
		const float& mass,
		const float& forceAbsValue,
		const float& theta,
		const float& ph,
		const glm::vec3& coordinates
	) : name(name), mass(mass), forceAbsValue(forceAbsValue), theta(theta), ph(ph), coordinates(coordinates)
	{
		updateCoordinateData();
		
		velocity = { 0.0f, 0.0f, 0.0f };

		trajectoryVertices = 0;

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
	}

	void computeInstantCharachteristics(const float& dt)
	{
		// Fx = F * sin(theta) * cos(ph)
		force.x = forceAbsValue * cos(glm::radians(theta)) * cos(glm::radians(ph));
		// Fy = F * sin(theta) * sin(ph)
		force.y = forceAbsValue * sin(glm::radians(theta)) * sin(glm::radians(ph));
		// Fz = F * cos(theta)
		force.z = forceAbsValue * cos(glm::radians(theta));

		// ax = Fx / m
		fullAcceleration.x = force.x / mass;
		// ay = Fy / m
		fullAcceleration.y = force.y / mass;
		// az = Fz / m
		fullAcceleration.z = force.z / mass;

		//Vx = V0x + ax*dt
		velocity.x += fullAcceleration.x * dt;
		//Vy = V0y + ay*dt
		velocity.y += fullAcceleration.y * dt;
		//Vz = V0z + az*dt
		velocity.z += fullAcceleration.z * dt;

		//x = x0 + Vx*dt
		coordinates.x += velocity.x * dt;
		//y = y0 + Vy*dt
		coordinates.y += velocity.y * dt;
		//z = z0 + Vz*dt
		coordinates.z += velocity.z * dt;

		updateCoordinateData();
		++trajectoryVertices;
	}

	void drawTrajectory(Camera& camera, const GLuint& screenWidth, const GLuint& screenHeight)
	{
		updateBuffer();

		Shader mp("mp.vertexShader", "mp.fragmentShader");

		mp.Use();
		mp.setMatrix4("model", glm::mat4(1.0f));
		mp.setMatrix4("view", camera.GetViewMatrix());
		mp.setMatrix4("projection", glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, trajectoryVertices * 3);
		glBindVertexArray(0);
	}

	void updateBuffer()
	{
		std::string filename = name + "_coordinates.txt";
		std::ifstream input(filename);

		if (input.is_open())
		{
			const int arraySize = trajectoryVertices * 3;

			GLfloat* vertices = new GLfloat[arraySize];

			for (int i = 0; i < arraySize; ++i)
				input >> vertices[i];

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * arraySize, vertices, GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			delete[] vertices;
		}

		else std::cout << "BUFFER::NOT_UPDATED";
	}

	std::string getObjectName() { return name; }


	void printObjectCoordinates()
	{
		std::string file = name + "_coordinates.txt";
		std::ifstream input(file);

		std::string line;

		if (input.is_open())
		{
			while(std::getline(input, line))
			std::cout << line << std::endl;
		}
		else std::cout << "file not opened";

		input.close();
	}



private:
	// Update-functions
	void updateCoordinateData()
	{
		std::string file = name + "_coordinates.txt";
		std::ofstream output(file, std::ios::app);

		if (output.is_open())
			output << coordinates.x << ' ' << coordinates.y << ' ' << coordinates.z << std::endl;

		else
			std::cout << "ERROR::FILE::IS::NOT_OPENED\n";

		output.close();
	}


private:
	std::string name;

	float mass;

	glm::vec3 coordinates;
	int trajectoryVertices;

	float forceAbsValue;
	float theta, ph;

	glm::vec3 force;
	glm::vec3 velocity;
	glm::vec3 fullAcceleration;

	GLuint VBO, VAO;

};