#include "Cube.h"

// When you specify cube vertices, you always define the vertices
// in a counterclockwise fasion, as if facing them. For example,
// for the back face, pretend you are looking at the backface, and
// go counterclockwise.

const float Tile::vertices[216] = {
    // Back Face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    // Front face
    -0.5f,  0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, 0.5f,  0.0f,  0.0f, 1.0f,


    // Left face
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

     // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,

    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

Tile::Tile(glm::mat4 transform, glm::vec3 color, float phase, bool isWater) 
    : m_transform(transform), m_color(color), m_phase(phase), m_isWater(isWater)
{}

void Tile::updateTransform(const glm::mat4& trsnfm) {
    m_transform = trsnfm;
}