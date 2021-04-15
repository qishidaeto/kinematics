#pragma once

struct CoordinateSystem
{
	CoordinateSystem()
	{
		GLfloat vertices[] =
		{
			// Axis X
		   -100000.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			100000.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

			// Axis Y
			0.0f, -100000.f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f,  100000.f, 0.0f, 0.0f, 1.0f, 0.0f,

			// Axis Z
			0.0f, 0.0f, -100000.f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f,  100000.f, 0.0f, 0.0f, 1.0f
		};

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw(const Shader& shader)
	{
		glBindVertexArray(VAO);
		shader.setVector3("color", glm::vec3(1.0f, 0.0f, 0.0f));
		glDrawArrays(GL_LINE_STRIP, 0, 2);

		shader.setVector3("color", glm::vec3(0.0f, 1.0f, 0.0f));
		glDrawArrays(GL_LINE_STRIP, 2, 2);

		shader.setVector3("color", glm::vec3(0.0f, 0.0f, 1.0f));
		glDrawArrays(GL_LINE_STRIP, 4, 2);
		glBindVertexArray(0);
	}

	~CoordinateSystem()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	GLuint VAO, VBO;
};