#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

Camera::Camera(glm::vec3 pos) {
	this->m_pos = pos;
    this->m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    this->m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->updateCameraVectors();
}

void Camera::setPosition(glm::vec3 pos) {
	this->m_pos = pos;
}

void Camera::processMovement(GLFWwindow* window, float deltaTime) {
    const float cameraSpeed = m_camSpeed * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_pos += cameraSpeed * m_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_pos -= cameraSpeed * m_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_pos -= cameraSpeed * m_right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_pos += cameraSpeed * m_right;
}

void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

glm::mat4 Camera::getViewMat() {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}