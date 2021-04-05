#pragma once

//GLM
#include <glm/vec3.hpp>

#include <fstream>
#include <iostream>
#include <string>

#include <math.h>

#define GLEW_STATIC
#include <GLEW/glew.h>

class MaterialPoint
{
public:
	MaterialPoint(
		std::string new_name,
		const float& new_mass,
		const glm::vec3& new_force,
		const glm::vec3& new_coordinates
	)
	{
		name = new_name;
		mass = new_mass;
		force = new_force;

		coordinates = new_coordinates;
		updateCoordinateData();

		velocity.x = 0.0f;
		velocity.y = 0.0f;
		velocity.z = 0.0f;

		distance = 0;
		trajectoryVertices = 0;

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
	}

	void computeInstantCharachteristics(const float& dt)
	{
		fullAcceleration.x = force.x / mass;
		fullAcceleration.y = force.y / mass;
		fullAcceleration.z = force.z / mass;

		//Vx = V0x + ax*dt
		velocity.x += fullAcceleration.x * dt;
		velocity.y += fullAcceleration.y * dt;
		velocity.z += fullAcceleration.z * dt;

		float x0 = coordinates.x;
		float y0 = coordinates.y;
		float z0 = coordinates.z;

		//x = x0 + Vx*dt
		coordinates.x += velocity.x * dt;
		coordinates.y += velocity.y * dt;
		coordinates.z += velocity.z * dt;
		updateCoordinateData();
		++trajectoryVertices;

		distance += sqrt(pow(coordinates.x - x0, 2) + pow(coordinates.y - y0, 2) + pow(coordinates.z - z0, 2));
	}

	void drawTrajectory()
	{
		updateBuffer();
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
	float distance;

	glm::vec3 coordinates;
	int trajectoryVertices;

	glm::vec3 force;
	glm::vec3 velocity;
	glm::vec3 fullAcceleration;

	GLuint VBO, VAO;

};