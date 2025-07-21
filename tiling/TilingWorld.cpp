#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TilingWorld.h"
#include "../common/PerlinNoise.hpp"
#include "../shapes/Cube.h"

TilingWorld::TilingWorld(int tiling_rows, int tiling_cols, int tiling_height, int omega, float amplitude)
	: TILING_ROWS(tiling_rows), TILING_COLS(tiling_cols), TILING_HEIGHT(tiling_height), OMEGA(omega), AMPLITUDE(amplitude)
{
    Cube cube;
    glGenBuffers(1, &tileVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube.m_vertices), cube.m_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenBuffers(1, &terrainTileTransformsVBO);
    glGenBuffers(1, &terrainTileColorsVBO);
    glGenBuffers(1, &waterTileTransformsVBO);
    glGenBuffers(1, &waterTileColorsVBO);

    // Setting up VAO's for each type of tile
    glGenVertexArrays(1, &terrainTileVAO);
    glGenVertexArrays(1, &waterTileVAO);

    // Setup terrainTileVAO
    glBindVertexArray(terrainTileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glEnableVertexAttribArray(0);               // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, terrainTileColorsVBO);
    glEnableVertexAttribArray(1);               // aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glVertexAttribDivisor(1, 1);
    glBindBuffer(GL_ARRAY_BUFFER, terrainTileTransformsVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);       // aTransform
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(3 + i, 1);
    }

    // Setup waterTileVAO
    glBindVertexArray(waterTileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glEnableVertexAttribArray(0);               // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, waterTileColorsVBO);
    glEnableVertexAttribArray(1);               // aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glVertexAttribDivisor(1, 1);
    glBindBuffer(GL_ARRAY_BUFFER, waterTileTransformsVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);       // aTransform
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(3 + i, 1);
    }

    // Reset binds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TilingWorld::generateWorld(int seed) {
    // Perlin Noise
    PerlinNoise2D pn;
    for (int i = 0; i < TILING_ROWS; i++) {
        for (int j = 0; j < TILING_COLS; j++) {
            float frequency = 0.02f; // smaller = smoother
            float amplitude = 1.0f;
            float height = 0.0f;
            for (int o = 0; o < 4; o++) {
                height += pn.eval(glm::vec2(i * frequency, j * frequency)) * amplitude;
                frequency *= 2.0f;
                amplitude *= 0.5f;
            }
            tilingHeightmap.push_back(
                static_cast<int>(glm::abs(height) * TILING_HEIGHT)
            );
        }
    }
    std::cout << "Created heightmap from Perlin Noise" << std::endl;

    int NUM_CUBES = 0;
    for (int i = 0; i < TILING_ROWS; i++) {
        for (int k = 0; k < TILING_COLS; k++) {
            int height = tilingHeightmap[TILING_ROWS * i + k];
            float relativeHeight = static_cast<float>(height) / TILING_HEIGHT;
            int j = 0;
            for (; j < height; j++) {
                terrainTileTransforms.push_back(
                    glm::translate(
                        glm::mat4(),
                        glm::vec3(
                            static_cast<float>(i),
                            static_cast<float>(j),
                            static_cast<float>(k)
                        )
                    )
                );
                float relativeHeight = static_cast<float>(j) / TILING_HEIGHT;
                terrainTileColors.push_back(getTileColor(relativeHeight));
                NUM_CUBES++;
            }
            if (height < WATER_LEVEL * TILING_HEIGHT) {
                for (; j < static_cast<int>(WATER_LEVEL * TILING_HEIGHT) - 1; j++) {
                    terrainTileTransforms.push_back(
                        glm::translate(
                            glm::mat4(),
                            glm::vec3(
                                static_cast<float>(i),
                                static_cast<float>(j),
                                static_cast<float>(k)
                            )
                        )
                    );
                    terrainTileColors.push_back(WATER);
                    NUM_CUBES++;
                }
                waterTileTransforms.push_back(
                    glm::translate(
                        glm::mat4(),
                        glm::vec3(
                            static_cast<float>(i),
                            static_cast<float>(j),
                            static_cast<float>(k)
                        )
                    )
                );
                waterTilePhases.push_back(static_cast<float>(rand()) / RAND_MAX);
                waterTileColors.push_back(WATER);
                NUM_CUBES++;
            }
        }
    }
    std::cout << "Created my cube transforms list with " << NUM_CUBES << " cubes." << std::endl;

    // Transforms -> GPU
    glBindBuffer(GL_ARRAY_BUFFER, terrainTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainTileTransforms.size() * sizeof(glm::mat4), terrainTileTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, waterTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, waterTileTransforms.size() * sizeof(glm::mat4), waterTileTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout << "Moved transforms to GPU in VBOs." << std::endl;
    
    // Colors -> GPU
    glBindBuffer(GL_ARRAY_BUFFER, terrainTileColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainTileColors.size() * sizeof(glm::vec3), terrainTileColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, waterTileColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, waterTileColors.size() * sizeof(glm::vec3), waterTileColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout << "Moved colors to GPU in VBOs." << std::endl;
}

void TilingWorld::animateWater(float timeDiff) {
    // Update sinusoidal transforms of cubes
    for (int i = 0; i < waterTileTransforms.size(); i++) {
        float sin_height = AMPLITUDE * glm::sin(OMEGA * (timeDiff + waterTilePhases[i]) + waterTilePhases[i]) + AMPLITUDE + WATER_LEVEL * TILING_HEIGHT - 1;
        waterTileTransforms[i][3][1] = sin_height;
    }
    glBindBuffer(GL_ARRAY_BUFFER, waterTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, waterTileTransforms.size() * sizeof(glm::mat4), waterTileTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TilingWorld::renderTiles() {
    glBindVertexArray(terrainTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, terrainTileTransforms.size());
    glBindVertexArray(waterTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, waterTileTransforms.size());
}

glm::vec3 TilingWorld::getTileColor(float height) {
    if (height >= SNOW_LEVEL) {
        return SNOW;
    }
    else if (height >= ROCK_LEVEL) {
        return ROCK;
    }
    else if (height >= GRASS_LEVEL) {
        return GRASS;
    }
    else if (height >= DIRT_LEVEL) {
        return DIRT;
    }
    else {
        return SAND;
    }
}