#pragma once

#include <vector>

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


enum forceVector {
	RAISE_DEVELOPED_FORCE_VECTOR,
	LOWER_DEVELOPED_FORCE_VECTOR,
	TURN_LEFT_DEVELOPED_FORCE_VECTOR,
	TURN_RIGHT_DEVELOPED_FORCE_VECTOR,
	INCREASE_DEVELOPED_FORCE_VECTOR,
	DECREASE_DEVELOPED_FORCE_VECTOR
};

class MaterialPoint
{
public:
	MaterialPoint(
		std::string name,
		const float& mass,
		const glm::vec3& coordinates
	) : name(name), mass(mass), coordinates(coordinates)
	{
		updateTrajectoryCoordinates(coordinates);

		developedForce = { 0.0f, 0.0f, 0.0f };
		theta = 0.0f;
		ph = 0.0f;

		incidentFlowForce = { 0.0f, 0.0f, 0.0f };

		velocity = { 0.0f, 0.0f, 0.0f };
	}
   
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboardObject(forceVector key, GLfloat deltaTime)
	{
		GLfloat deltaForce = 3.0f;
		GLfloat deltaAngle = 1.0f;

		if (key == RAISE_DEVELOPED_FORCE_VECTOR)
			theta += deltaAngle;
		if (key == LOWER_DEVELOPED_FORCE_VECTOR)
			theta -= deltaAngle;
		if (key == TURN_LEFT_DEVELOPED_FORCE_VECTOR)
			ph += deltaAngle;
		if (key == TURN_RIGHT_DEVELOPED_FORCE_VECTOR)
			ph -= deltaAngle;

		if (key == INCREASE_DEVELOPED_FORCE_VECTOR)
			forceAbsValue += deltaForce;
		if (key == DECREASE_DEVELOPED_FORCE_VECTOR)
		{
			forceAbsValue -= deltaForce;

			if (forceAbsValue <= 0.0f)
				forceAbsValue = 0.0f;
		}
	}

	void printObjectCharachteristics()
	{
		std::cout << "\t\t Object name: " << name << std::endl;
		std::cout << "forceAbsValue = " << forceAbsValue << std::endl;
		std::cout << "Zenith = " << theta << ", " << "azimuth = " << ph;

		system("cls");
	}

	void computeInstantCharachteristics(const float& ambientDensity, const float& dt)
	{
		//Frx = q * Vx^2 / 2
		incidentFlowForce.x = ambientDensity * velocity.x * velocity.x / 2;
		//Fry = q * Vy^2 / 2
		incidentFlowForce.y = ambientDensity * velocity.y * velocity.y / 2;
		//Frz = q * Vz^2 / 2
		incidentFlowForce.z = ambientDensity * velocity.z * velocity.z / 2;

		// Fx = F * cos(theta) * sin(ph)
		developedForce.x = forceAbsValue * cos(glm::radians(theta)) * sin(glm::radians(ph));
		// Fy = F * sin(theta)
		developedForce.y = forceAbsValue * sin(glm::radians(theta));
		// Fz = F * cos(theta) * cos(ph)
		developedForce.z = forceAbsValue * cos(glm::radians(theta)) * cos(glm::radians(ph));

		// ax = (Fx + Frx) / m
		fullAcceleration.x = (developedForce.x - incidentFlowForce.x) / mass;
		// ay = (Fy + Fry) / m
		fullAcceleration.y = (developedForce.y - incidentFlowForce.y) / mass;
		// az = (Fz + Frz) / m
		fullAcceleration.z = (developedForce.z - incidentFlowForce.z) / mass;

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

		updateTrajectoryCoordinates(coordinates);
	}

	void drawTrajectory(const Shader& shader)
	{
		GLuint VAO;
		GLuint VBO;

		glGenBuffers(1, &VAO);
		glGenVertexArrays(1, &VBO);

		updateTrajectroyCoordinatesBuffer(VAO, VBO);

		shader.setVector3("color", glm::vec3(1.0f, 1.0f, 0.0f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, trajectoryCoordinates.size() / 3);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void drawIncidentFlowForceVector(const Shader& shader)
	{
		GLuint VAO;
		GLuint VBO;

		glGenBuffers(1, &VAO);
		glGenVertexArrays(1, &VBO);
		updateForceCoordinatesBuffer(VAO, VBO, -incidentFlowForce);

		shader.setVector3("color", glm::vec3(0.545f, 0.0f, 0.545f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void drawDevelopedForceVector(const Shader& shader)
	{
		GLuint VAO;
		GLuint VBO;

		glGenBuffers(1, &VAO);
		glGenVertexArrays(1, &VBO);
		updateForceCoordinatesBuffer(VAO, VBO, developedForce);

		shader.setVector3("color", glm::vec3(0.0f, 0.392f, 0.0f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}


	std::string getObjectName() const
	{
		return name;
	}

	glm::vec3 getObjectCoordinates() const
	{
		return coordinates;
	}

	float getObjectTheta() const
	{
		return theta;
	}

	float getObjectPh() const
	{
		return ph;
	}

	~MaterialPoint()
	{

	}

private:
	void updateTrajectoryCoordinates(const glm::vec3& coordinates)
	{
		trajectoryCoordinates.push_back(coordinates.x);
		trajectoryCoordinates.push_back(coordinates.y);
		trajectoryCoordinates.push_back(coordinates.z);
	}
	void updateTrajectroyCoordinatesBuffer(const GLuint& VAO, const GLuint& VBO) const
	{
		GLfloat* vertices = new GLfloat[trajectoryCoordinates.size()];

		for (int i = 0; i < trajectoryCoordinates.size(); ++i)
			vertices[i] = trajectoryCoordinates[i];

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * trajectoryCoordinates.size(), vertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		delete[] vertices;
	}
	void updateForceCoordinatesBuffer(const GLuint& VAO, const GLuint& VBO, const glm::vec3& force) const
	{
		GLfloat* forceVectorVertices = new GLfloat[6];

		forceVectorVertices[0] = coordinates.x;
		forceVectorVertices[1] = coordinates.y;
		forceVectorVertices[2] = coordinates.z;

		forceVectorVertices[3] = coordinates.x + force.x;
		forceVectorVertices[4] = coordinates.y + force.y;
		forceVectorVertices[5] = coordinates.z + force.z;

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(forceVectorVertices) * 6, forceVectorVertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		delete[] forceVectorVertices;
	}

private:
	std::string name;
	float mass;

	float forceAbsValue;
	float theta, ph;

	std::vector<GLfloat> trajectoryCoordinates;
	glm::vec3 coordinates;

	glm::vec3 developedForce;
	glm::vec3 incidentFlowForce;

	glm::vec3 velocity;
	glm::vec3 fullAcceleration;
};