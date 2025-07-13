#include <glm/glm.hpp>
#include "Camera.h"

Camera::Camera(glm::vec3 pos) {
	this->m_pos = pos;
}

void Camera::setPosition(glm::vec3 pos) {
	this->m_pos = pos;
}