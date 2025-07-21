#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../shapes/Cube.h"

const float WATER_LEVEL = 0.2f;
const float SAND_LEVEL = 0.25f;
const float DIRT_LEVEL = 0.35f;
const float GRASS_LEVEL = 0.4f;
const float ROCK_LEVEL = 0.6f;
const float SNOW_LEVEL = 0.8f;
const glm::vec3 WATER = glm::vec3(0.0f, 0.0f, 150.0f / 255.0f);
const glm::vec3 SAND = glm::vec3(237.0f / 255.0f, 201.0f / 255.0f, 175.0f / 255.0f);
const glm::vec3 DIRT = glm::vec3(155.0f / 255.0f, 118.0f / 255.0f, 83.0f / 255.0f);
const glm::vec3 GRASS = glm::vec3(34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f);
const glm::vec3 ROCK = glm::vec3(120.0f / 255.0f, 120.0f / 255.0f, 120.0f / 255.0f);
const glm::vec3 SNOW = glm::vec3(1.0f, 1.0f, 1.0f);

class TilingWorld {
public:
	TilingWorld(int tiling_rows, int tiling_cols, int tiling_height, int omega, float amplitude);

	void generateWorld(int seed);

	void animateWater(float timeDiff);

	void renderTiles();

private:
	glm::vec3 getTileColor(float height);
	
	// Updates GPU buffers
	void updateTerrainTileTransformsGPU();
	void updateWaterTileTransformsGPU();
	void updateTerrainTileColorsGPU();
	void updateWaterTileColorsGPU();

	// World attributes
	int TILING_ROWS = 70;
	int TILING_COLS = 70;
	int TILING_HEIGHT = 40;
	float OMEGA = 5.0f;
	float AMPLITUDE = 1.0f;
	int NUM_TILES = 0;

	// All non water blocks

	std::vector<Tile> terrainTiles;
	std::vector<Tile> waterTiles;
	
	// Transforms for non-water and water tiles.
	std::vector<glm::mat4> terrainTileTransforms;
	std::vector<glm::mat4> waterTileTransforms;

	// Colors for non-water and water tiles.
	std::vector<glm::vec3> terrainTileColors;
	std::vector<glm::vec3> waterTileColors;
	std::vector<float> waterTilePhases;

	// World heightmap
	std::vector<int> tilingHeightmap;

	// Responsible for binding of buffers and rendering
	GLuint tileVBO; // For the standard cube (layout == 1).

	// Store all transform matrices in these guys
	GLuint terrainTileVAO, waterTileVAO;
	GLuint terrainTileTransformsVBO, waterTileTransformsVBO;
	GLuint terrainTileColorsVBO, waterTileColorsVBO;
};