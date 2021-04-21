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

// Check status code
#define TRAJECTORY          1
#define DEVELOPED_FORCE     2
#define DRAG_FORCE          3
#define GRAVITATIONAL_FORCE 4

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
		const std::string& name,
		const std::string& form,
		const float& mass,
		const float& dragCoefficient,
		const float& midsection,
		const glm::vec3& coordinates
	) : name(name), mass(mass), coordinates(coordinates), dragCoefficient(dragCoefficient), midsection(midsection), form(form)
	{
		updateTrajectoryCoordinates(coordinates);

		forceAbsValue = 0.0f;
		theta = 0.0f;
		ph = 0.0f;

		developedForce = { 0.0f, 0.0f, 0.0f };
		dragForce = { 0.0f, 0.0f, 0.0f };
		gravitationalForce = { 0.0f, 0.0f, 0.0f };

		velocity = { 0.0f, 0.0f, 0.0f };
		acceleration = { 0.0f, 0.0f, 0.0f };
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
	void computeInstantCharachteristics(const std::vector<MaterialPoint> objects, const float& ambientDensity, const float& dt)
	{
		gravitationalForce = { 0.0f, 0.0f, 0.0f };

		const float gravitationalConstant = 6.6743e-11f;

		for (const MaterialPoint& object : objects)
			if (object.name != this->name)
				gravitationalForce += gravitationalConstant * object.mass * mass / glm::length(object.coordinates - coordinates) * glm::normalize(object.coordinates - coordinates);

		//Frx = Cf * q * Vx^2 / 2 * S
		dragForce.x = dragCoefficient * ambientDensity * velocity.x * velocity.x / 2 * midsection;
		//Fry = Cf * q * Vy^2 / 2 * S
		dragForce.y = dragCoefficient * ambientDensity * velocity.y * velocity.y / 2 * midsection;
		//Frz = Cf * q * Vz^2 / 2 * S
		dragForce.z = dragCoefficient * ambientDensity * velocity.z * velocity.z / 2 * midsection;

		// Fx = F * cos(theta) * sin(ph)
		developedForce.x = forceAbsValue * cos(glm::radians(theta)) * sin(glm::radians(ph));
		// Fy = F * sin(theta)
		developedForce.y = forceAbsValue * sin(glm::radians(theta));
		// Fz = F * cos(theta) * cos(ph)
		developedForce.z = forceAbsValue * cos(glm::radians(theta)) * cos(glm::radians(ph));

		// ax = (Fx - Fdx + Fgx) / m
		acceleration.x = (developedForce.x - dragForce.x + gravitationalForce.x) / mass;
		// ay = (Fy - Fdy + Fgy) / m
		acceleration.y = (developedForce.y - dragForce.y + gravitationalForce.y) / mass;
		// az = (Fz - Fdz + Fgz) / m
		acceleration.z = (developedForce.z - dragForce.z + gravitationalForce.z) / mass;

		//Vx = V0x + ax * dt
		velocity.x += acceleration.x * dt;
		//Vy = V0y + ay * dt
		velocity.y += acceleration.y * dt;
		//Vz = V0z + az * dt
		velocity.z += acceleration.z * dt;

		//x = x0 + Vx * dt
		coordinates.x += velocity.x * dt;
		//y = y0 + Vy * dt
		coordinates.y += velocity.y * dt;
		//z = z0 + Vz * dt
		coordinates.z += velocity.z * dt;

		updateTrajectoryCoordinates(coordinates);
	}

	// Draw-functions
	void drawTrajectory(const Shader& shader)
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

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, trajectoryCoordinates.size() / 3);
		glBindVertexArray(0);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
	void drawDragForceVector(const Shader& shader)
	{
		GLuint VAO;
		GLuint VBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		updateForceCoordinatesBuffer(VAO, VBO, -dragForce);

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

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		updateForceCoordinatesBuffer(VAO, VBO, developedForce);

		shader.setVector3("color", glm::vec3(0.0f, 0.392f, 0.0f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void drawGravitationalForceVector(const Shader& shader)
	{
		GLuint VAO;
		GLuint VBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		updateForceCoordinatesBuffer(VAO, VBO, gravitationalForce);

		shader.setVector3("color", glm::vec3(0.416f, 0.353f, 0.804f));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}


	// Get-functions
	bool getDrawStatus(const unsigned short int& code) const
	{
		if (code == TRAJECTORY)
			return drawTrajectoryStatus;

		if (code == DEVELOPED_FORCE)
			return drawDevelopedForceStatus;

		if (code == DRAG_FORCE)
			return drawDragForceStatus;

		if (code == GRAVITATIONAL_FORCE)
			return drawGravitationalStatus;
	}
	std::string getObjectName() const { return name; }
	std::string getObjectForm() const { return form; }
	glm::vec3 getObjectCoordinates() const { return coordinates; }
	glm::vec3 getObjectDevelopedForceVector() const { return developedForce; }
	glm::vec3 getObjectDragForceVector() const { return dragForce; }
	glm::vec3 getObjectGravitationalForceVector() const { return gravitationalForce; }
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
private:
	bool drawTrajectoryStatus;
	bool drawDevelopedForceStatus;
	bool drawDragForceStatus;
	bool drawGravitationalStatus;

	std::string name;
	std::string form;
	float dragCoefficient;


	float theta, ph;

	std::vector<GLfloat> trajectoryCoordinates;
	glm::vec3 coordinates;

	glm::vec3 developedForce;
	glm::vec3 dragForce;
	glm::vec3 gravitationalForce;

	glm::vec3 velocity;
	glm::vec3 acceleration;
};