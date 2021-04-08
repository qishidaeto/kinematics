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


enum Object_Movement {
    UP_OBJECT,
    DOWN_OBJECT,
    TURN_LEFT_OBECT,
    TURN_RIGHT_OBJECT,
    INCREASE_FORCE_OBJECT,
    DECREASE_FORCE_OBJECT
};

class MaterialPoint
{
public:
    MaterialPoint(
        std::string name,
        const float& mass,
        const glm::vec3& coordinates
    ) : name(name), mass(mass), coordinates(coordinates), mp("mp.vertexShader", "mp.fragmentShader")
    {
        updateTrajectoryCoordinates(coordinates);

        force = { 0.0f, 0.0f, 0.0f };
        zenith = 0.0f;
        azimuth = 0.0f;

        velocity = { 0.0f, 0.0f, 0.0f };
    }
   
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboardObject(Object_Movement key, GLfloat deltaTime)
    {
        GLfloat delta = 0.3f;

        if (key == UP_OBJECT)
                zenith += delta;

        if (key == DOWN_OBJECT)
                zenith -= delta;

        if (key == TURN_LEFT_OBECT)
                azimuth -= delta;

        if (key == TURN_RIGHT_OBJECT)
                azimuth += delta;

        if (key == INCREASE_FORCE_OBJECT)
            forceAbsValue += delta;

        if (key == DECREASE_FORCE_OBJECT)
            forceAbsValue -= delta;
    }

    void printObjectCharachteristics()
    {
        std::cout << "\t\t Object name: " << name << std::endl;
        std::cout << "forceAbsValue = " << forceAbsValue << std::endl;
        std::cout << "Zenith = " << zenith << ", " << "azimuth = " << azimuth;

        system("cls");
    }

    void computeInstantCharachteristics(const float& dt)
    {
        // Fx = F * sin(theta) * cos(ph)
        force.x = forceAbsValue * sin(glm::radians(zenith)) * cos(glm::radians(azimuth));
        // Fy = F * sin(theta) * sin(ph)
        force.y = forceAbsValue * sin(glm::radians(zenith)) * sin(glm::radians(azimuth));
        // Fz = F * cos(theta)
        force.z = forceAbsValue * cos(glm::radians(zenith));

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

        updateTrajectoryCoordinates(coordinates);
    }

    void drawObjectTrajectory(Camera& camera, const GLuint& screenWidth, const GLuint& screenHeight)
    {
        GLuint VAO;
        GLuint VBO;

        glGenBuffers(1, &VAO);
        glGenVertexArrays(1, &VBO);

        updateTrajectroyCoordinatesBuffer(VAO, VBO);

        mp.Use();
        mp.setMatrix4("model", glm::mat4(1.0f));
        mp.setMatrix4("view", camera.GetViewMatrix());
        mp.setMatrix4("projection", glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f));

        mp.setVector3("verticeColor", glm::vec3(1.0f, 1.0f, 0.0f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, trajectoryCoordinates.size() / 3);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void drawObjectForceVector(Camera& camera, const GLuint& screenWidth, const GLuint& screenHeight)
    {
        GLuint forceVector_VAO;
        GLuint forceVector_VBO;

        glGenBuffers(1, &forceVector_VAO);
        glGenVertexArrays(1, &forceVector_VBO);

        GLfloat* forceVectorVertices = new GLfloat[6];

        forceVectorVertices[0] = coordinates.x;
        forceVectorVertices[1] = coordinates.y;
        forceVectorVertices[2] = coordinates.z;

        forceVectorVertices[3] = coordinates.x + force.x;
        forceVectorVertices[4] = coordinates.y + force.y;
        forceVectorVertices[5] = coordinates.z + force.z;

        glBindVertexArray(forceVector_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, forceVector_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(forceVectorVertices) * 6, forceVectorVertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        delete[] forceVectorVertices;

        mp.Use();
        mp.setMatrix4("model", glm::mat4(1.0f));
        mp.setMatrix4("view", camera.GetViewMatrix());
        mp.setMatrix4("projection", glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f));

        mp.setVector3("verticeColor", glm::vec3(0.0f, 1.0f, 1.0f));

        glBindVertexArray(forceVector_VAO);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &forceVector_VAO);
        glDeleteBuffers(1, &forceVector_VBO);
    }

    void updateTrajectroyCoordinatesBuffer(const GLuint& VAO, const GLuint& VBO)
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

    std::string getObjectName() const
    {
        return name;
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


private:
    std::string name;
    float mass;

    float forceAbsValue;
    float zenith, azimuth;

    std::vector<GLfloat> trajectoryCoordinates;
    glm::vec3 coordinates;

    glm::vec3 force;
    glm::vec3 velocity;
    glm::vec3 fullAcceleration;

    Shader mp;

};