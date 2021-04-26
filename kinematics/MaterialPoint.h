#pragma once

// GLEW
#include <GL/glew.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm/vec3.hpp>
#include <glm/glm/trigonometric.hpp>

// Classes
#include "Shader.h"
#include "Camera.h"

#define EMPTY_SPACE 0
#define NEAR_AN_ASTRONOMICAL_OBJECT 1

#define GRAVITATIONAL_CONSTANT 6.6743e-11f

// Force vector control
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
		const std::string& id,
		const float& mass,
		const float& dragCoefficient,
		const float& midsection,
		const glm::vec3& coordinates
	) : id(id), mass(mass), dragCoefficient(dragCoefficient), midsection(midsection), coordinates(coordinates)
	{
		updateTrajectoryCoordinates(coordinates);

		forceAbsValue = 0.0f;
		theta = 0.0f;
		ph = 0.0f;

		developedForce = glm::vec3(0.0f);
		dragForce = glm::vec3(0.0f);
		gravitationalForce = glm::vec3(0.0f);
		normalReactionForce = glm::vec3(0.0f);

		velocity = glm::vec3(0.0f);
		acceleration = glm::vec3(0.0f);

		drawTrajectoryStatus = true;
		drawDevelopedForceStatus = true;
		drawDragForceStatus = true;
		drawGravitationalForceStatus = true;
	}
   
	// Object control-function
	void ProcessKeyboardObject(forceVector key, GLfloat deltaTime)
	{
		GLfloat deltaForce = 1.0f;
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

	// Compute charachteristics
	void computeInstantCharachteristics(
		const std::vector<MaterialPoint> objects, 
		const float& ambientDensity, 
		const float& dt, 
		const int& typeOfSpace, 
		const float& astronomicalObjectMass,
		const float& astronomicalObjectRadius,
		const float& astronomicalObjectAverageSoilDensity)
	{
		if (typeOfSpace == EMPTY_SPACE)
		{
			gravitationalForce = { 0.0f, 0.0f, 0.0f };

			for (const MaterialPoint& object : objects)
				if (object.id != this->id)
					gravitationalForce += GRAVITATIONAL_CONSTANT * object.mass * mass /
					glm::length(object.coordinates - coordinates) * glm::normalize(object.coordinates - coordinates);

			if (glm::length(velocity) != 0.0f)
				dragForce = -glm::normalize(velocity) * (dragCoefficient * ambientDensity * glm::length(velocity) * glm::length(velocity) / 2 * midsection);
		}

		if (typeOfSpace == NEAR_AN_ASTRONOMICAL_OBJECT)
		{
			gravitationalForce = glm::vec3(0.0f, -1.0f, 0.0f) * GRAVITATIONAL_CONSTANT * mass * astronomicalObjectMass /
				((astronomicalObjectRadius + coordinates.y) * (astronomicalObjectRadius + coordinates.y));

				// When the object hits the surface
				if (coordinates.y < 0.0f)
				{
					if (glm::length(velocity) != 0.0f)
						dragForce = -glm::normalize(velocity) * (dragCoefficient * astronomicalObjectAverageSoilDensity * glm::length(velocity) * glm::length(velocity) / 2 * midsection);
					normalReactionForce = glm::vec3(0.0f, 1.0f, 0.0f) * mass * GRAVITATIONAL_CONSTANT * astronomicalObjectMass / ((astronomicalObjectRadius) * (astronomicalObjectRadius));
				}

				// When the object does not touch the surface
				else
				{
					if (glm::length(velocity) != 0.0f)
						dragForce = -glm::normalize(velocity) * (dragCoefficient * ambientDensity * glm::length(velocity) * glm::length(velocity) / 2 * midsection);
					normalReactionForce = glm::vec3(0.0f, 0.0f, 0.0f);
				}
		}

		// Fx = F * cos(theta) * sin(ph)
		developedForce.x = forceAbsValue * cos(glm::radians(theta)) * sin(glm::radians(ph));
		// Fy = F * sin(theta)
		developedForce.y = forceAbsValue * sin(glm::radians(theta));
		// Fz = F * cos(theta) * cos(ph)
		developedForce.z = forceAbsValue * cos(glm::radians(theta)) * cos(glm::radians(ph));

		acceleration = (developedForce + dragForce + gravitationalForce + normalReactionForce) / mass;
		velocity += acceleration * dt;
		coordinates += velocity * dt;

		updateTrajectoryCoordinates(coordinates);
		}


	// Draw-functions
	void drawTrajectory(const Shader& shader)
	{
		if (drawTrajectoryStatus)
		{
			GLuint VAO;
			GLuint VBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * trajectoryCoordinates.size(), &trajectoryCoordinates[0], GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			shader.setVector3("color", glm::vec3(1.0f, 1.0f, 0.0f));
			glLineWidth(2.0f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, trajectoryCoordinates.size() / 3);
			glBindVertexArray(0);

			glDeleteBuffers(1, &VBO);
			glDeleteVertexArrays(1, &VAO);
		}
	}
	void drawDragForceVector(const Shader& shader)
	{
		if (drawDragForceStatus)
		{
			GLuint VAO;
			GLuint VBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			updateForceCoordinatesBuffer(VAO, VBO, dragForce);

			shader.setVector3("color", glm::vec3(0.545f, 0.0f, 0.545f));
			glLineWidth(5.0f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	}
	void drawDevelopedForceVector(const Shader& shader)
	{
		if (drawDevelopedForceStatus)
		{
			GLuint VAO;
			GLuint VBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			updateForceCoordinatesBuffer(VAO, VBO, developedForce);

			shader.setVector3("color", glm::vec3(0.0f, 0.392f, 0.0f));
			glLineWidth(2.0f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	}
	void drawGravitationalForceVector(const Shader& shader)
	{
		if (drawGravitationalForceStatus)
		{
			GLuint VAO;
			GLuint VBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			updateForceCoordinatesBuffer(VAO, VBO, gravitationalForce);

			shader.setVector3("color", glm::vec3(0.416f, 0.353f, 0.804f));
			glLineWidth(2.0f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	}

	// Get-functions
	std::string getObjectName() const { return id; }
	glm::vec3 getObjectCoordinates() const { return coordinates; }
	glm::vec3 getObjectVelocityVector() const { return velocity; }
	glm::vec3 getObjectAccelerationVector() { return acceleration; }
	glm::vec3 getObjectDevelopedForceVector() const { return developedForce; }
	glm::vec3 getObjectDragForceVector() const { return dragForce; }
	glm::vec3 getObjectGravitationalForceVector() const { return gravitationalForce; }
	glm::vec3 getObjectNormalReactionForceVector() { return normalReactionForce; }
	float getObjectMass() const { return mass; }
	float getObjectDragCoefficient() const { return dragCoefficient; }
	float getObjectMidsection() const { return midsection; }


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
public:
	float mass;
	float midsection;

	float forceAbsValue;
	float theta, ph;


	bool drawTrajectoryStatus;
	bool drawDevelopedForceStatus;
	bool drawDragForceStatus;
	bool drawGravitationalForceStatus;

private:
	std::string id;
	std::string form;
	float dragCoefficient;



	std::vector<GLfloat> trajectoryCoordinates;
	glm::vec3 coordinates;

	glm::vec3 developedForce;
	glm::vec3 dragForce;
	glm::vec3 gravitationalForce;
	glm::vec3 normalReactionForce;

	glm::vec3 velocity;
	glm::vec3 acceleration;
};