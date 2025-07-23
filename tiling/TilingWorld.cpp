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
    glGenBuffers(1, &tileVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Tile::vertices), Tile::vertices, GL_STATIC_DRAW);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(2);               // aNormal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(2);               // aNormal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
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
    PerlinNoise2D pn(seed);
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
    /*for (int i = 0; i < TILING_ROWS; i++) {
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
    }*/
    for (int i = 0; i < TILING_ROWS; i++) {
        for (int k = 0; k < TILING_COLS; k++) {
            int height = tilingHeightmap[TILING_ROWS * i + k];
            float relativeHeight = static_cast<float>(height) / TILING_HEIGHT;
            int j = 0;
            for (; j < height; j++) {
                glm::mat4 trnfm = glm::translate(
                    glm::mat4(),
                    glm::vec3(
                        static_cast<float>(i),
                        static_cast<float>(j),
                        static_cast<float>(k)
                    )
                );
                glm::vec3 color = getTileColor(static_cast<float>(j) / TILING_HEIGHT);
                terrainTiles.push_back(
                    Tile(trnfm, color, 0.0f, false)
                );
                NUM_CUBES++;
            }
            if (height < WATER_LEVEL * TILING_HEIGHT) {
                for (; j < static_cast<int>(WATER_LEVEL * TILING_HEIGHT) - 1; j++) {
                    glm::mat4 trnfm = glm::translate(
                        glm::mat4(),
                        glm::vec3(
                            static_cast<float>(i),
                            static_cast<float>(j),
                            static_cast<float>(k)
                        )
                    );
                    terrainTiles.push_back(
                        Tile(trnfm, WATER, 0.0f, false)
                    );
                    NUM_CUBES++;
                }
                glm::mat4 trnfm = glm::translate(
                    glm::mat4(),
                    glm::vec3(
                        static_cast<float>(i),
                        static_cast<float>(j),
                        static_cast<float>(k)
                    )
                );
                float phase = static_cast<float>(rand()) / RAND_MAX;
                waterTiles.push_back(
                    Tile(trnfm, WATER, phase, true)
                );
                NUM_CUBES++;
            }
        }
    }
    std::cout << "Created my cube transforms list with " << NUM_CUBES << " cubes." << std::endl;

    // Transforms -> GPU
    updateTerrainTileTransformsGPU();
    updateTerrainTileColorsGPU();
    /*glBindBuffer(GL_ARRAY_BUFFER, terrainTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainTileTransforms.size() * sizeof(glm::mat4), terrainTileTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, waterTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, waterTileTransforms.size() * sizeof(glm::mat4), waterTileTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    std::cout << "Moved transforms to GPU in VBOs." << std::endl;
    
    // Colors -> GPU
    updateWaterTileTransformsGPU();
    updateWaterTileColorsGPU();
    /*glBindBuffer(GL_ARRAY_BUFFER, terrainTileColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainTileColors.size() * sizeof(glm::vec3), terrainTileColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, waterTileColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, waterTileColors.size() * sizeof(glm::vec3), waterTileColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    std::cout << "Moved colors to GPU in VBOs." << std::endl;
}

void TilingWorld::animateWater(float timeDiff) {
    // Update sinusoidal transforms of cubes
    for (Tile& t : waterTiles) {
        float h = AMPLITUDE * glm::sin(OMEGA * (timeDiff + t.m_phase) + t.m_phase) + AMPLITUDE + WATER_LEVEL * TILING_HEIGHT - 1;
        t.m_transform[3][1] = h;
    }
    
    updateWaterTileTransformsGPU();
}

void TilingWorld::renderTiles() {
    glBindVertexArray(terrainTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, terrainTiles.size());
    glBindVertexArray(waterTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, waterTiles.size());
}

void TilingWorld::updateTerrainTileTransformsGPU() {
    std::vector<glm::mat4> trnfms;
    for (const Tile& t : terrainTiles) {
        trnfms.push_back(t.m_transform);
    }

    glBindBuffer(GL_ARRAY_BUFFER, terrainTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, trnfms.size() * sizeof(glm::mat4), trnfms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TilingWorld::updateWaterTileTransformsGPU() {
    std::vector<glm::mat4> trnfms;
    for (const Tile& t : waterTiles) {
        trnfms.push_back(t.m_transform);
    }

    glBindBuffer(GL_ARRAY_BUFFER, waterTileTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, trnfms.size() * sizeof(glm::mat4), trnfms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TilingWorld::updateTerrainTileColorsGPU() {
    std::vector<glm::vec3> colors;
    for (const Tile& t : terrainTiles) {
        colors.push_back(t.m_color);
    }

    glBindBuffer(GL_ARRAY_BUFFER, terrainTileColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TilingWorld::updateWaterTileColorsGPU() {
    std::vector<glm::vec3> colors;
    for (const Tile& t : waterTiles) {
        colors.push_back(t.m_color);
    }

    glBindBuffer(GL_ARRAY_BUFFER, waterTileColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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