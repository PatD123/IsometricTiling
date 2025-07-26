#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shapes/Cube.h"

class Light : public Tile {
public:
	Light(glm::mat4 transform, glm::vec3 color);

	void updateTransform(const glm::mat4& trnsfm) override;

	void updatePosition(const glm::vec3& pos);

	glm::vec3 translate(float dx, float dy, float dz);

	void updateColor();

	void draw();

private:
	GLuint lightVAO, lightVBO;

	const glm::vec3 basePosition;
	glm::vec3 m_position;
};