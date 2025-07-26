#include "Light.h"

Light::Light(glm::mat4 transform, glm::vec3 color)
	: Tile(transform, color, 0.0, false), m_position(glm::vec3(transform[3])), basePosition(glm::vec3(transform[3]))
{
	glGenBuffers(1, &lightVBO);
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tile::vertices), Tile::vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Light::updateTransform(const glm::mat4& trnsfm) {
	m_transform = trnsfm;
}

void Light::updatePosition(const glm::vec3& pos) {
	m_position = pos;
}

glm::vec3 Light::translate(float dx, float dy, float dz) {
	glm::vec3 newPosition = basePosition + glm::vec3(dx, dy, dz);
	updatePosition(newPosition);
	updateTransform(glm::translate(glm::mat4(), newPosition));

	return m_position;
}

void Light::updateColor() {

}

void Light::draw() {
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}