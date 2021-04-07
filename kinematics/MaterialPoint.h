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
    ) : name(name), mass(mass), forceAbsValue(forceAbsValue), theta(theta), ph(ph), coordinates(coordinates), mp("mp.vertexShader", "mp.fragmentShader")
    {
        updateTrajectoryCoordinates(coordinates);
        velocity = { 0.0f, 0.0f, 0.0f };
    }

    void computeInstantCharachteristics(const float& dt)
    {
        // Fx = F * sin(theta) * cos(ph)
        force.x = forceAbsValue * sin(glm::radians(theta)) * cos(glm::radians(ph));
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
        mp.setMatrix4("projection", glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, trajectoryCoordinates.size() / 3);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
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
    Shader mp;


    std::vector<GLfloat> trajectoryCoordinates;

    std::string name;

    float mass;

    glm::vec3 coordinates;

    float forceAbsValue;
    float theta, ph;

    glm::vec3 force;
    glm::vec3 velocity;
    glm::vec3 fullAcceleration;
};