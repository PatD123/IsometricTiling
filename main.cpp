#include <iostream>
#include <stdlib.h>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/ShaderHelper.h"
#include "common/PerlinNoise.hpp"
#include "shapes/Cube.h"
#include "camera/Camera.h"

// About tiling
const int NUM_CUBES = 100;
const int TILING_ROWS = 30;
const int TILING_COLS = 30;
const int TILING_HEIGHT = 15;
const float OMEGA = 5.0f;
const float AMPLITUDE = 0.2f;

// Each terrain type is associated with 
// - Elevation
// - Color
const float WATER_LEVEL = 0.0f;
const float SAND_LEVEL  = 0.0f;
const float DIRT_LEVEL  = 0.2f;
const float GRASS_LEVEL = 0.4f;
const float ROCK_LEVEL  = 0.5f;
const float SNOW_LEVEL  = 0.7f;
const glm::vec3 WATER = glm::vec3(0.0f, 0.0f, 150.0f / 255.0f);
const glm::vec3 SAND = glm::vec3(237.0f / 255.0f, 201.0f / 255.0f, 175.0f / 255.0f);
const glm::vec3 DIRT = glm::vec3(155.0f / 255.0f, 118.0f / 255.0f, 83.0f / 255.0f);
const glm::vec3 GRASS = glm::vec3(34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f);
const glm::vec3 ROCK = glm::vec3(120.0f / 255.0f, 120.0f / 255.0f, 120.0f / 255.0f);
const glm::vec3 SNOW = glm::vec3(1.0f, 1.0f, 1.0f);

// Timing
float PROGRAM_START_TIME = glfwGetTime();
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void processInput(GLFWwindow* window);

// Making Camera
Camera cam(glm::vec3(0.0f, 0.0f, 0.0f));
const float radius = 5.0f;
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Compile and link shaders
    ShaderHelper sh;
    GLuint shaderProgram = sh.compileShaders("VertexShader.glsl", "FragmentShader.glsl");

    glUseProgram(shaderProgram);

    // Enable z-buffer depth testing
    glEnable(GL_DEPTH_TEST);

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Sidenote on instancing
    /*
    glDrawArraysInstanced (or whatever it is called), you specify what your instance is, which 
    if we want to draw cubes is 36 vertices, so that is now considered an "instance". We then
    specify numInstances, which is self-explanatory. The VertexDiv function tells OpenGL how
    often I should be switching to a new Matrix to transform our cube. We want it so that 
    for each instance of a cube, we use a new matrix. That's what it does.
    */

    // Transform and Color instancing
    std::vector<glm::mat4> cubeTransforms;
    /*glm::mat4 cubeTransforms[TILING_ROWS * TILING_COLS * TILING_HEIGHT];*/
    std::vector<glm::vec3> cubeColors;
    /*glm::vec3 cubeColors[TILING_ROWS * TILING_COLS * TILING_HEIGHT];*/
    std::vector<int> tilingHeightmap;
    /*int tilingHeightmap[TILING_ROWS * TILING_COLS];*/
    /*float cubePhases[TILING_ROWS * TILING_COLS * TILING_HEIGHT];*/

    // Perlin Noise
    PerlinNoise2D pn;
    for (int i = 0; i < TILING_ROWS; i++) {
        for (int j = 0; j < TILING_COLS; j++) {
            tilingHeightmap.push_back(
                static_cast<int>(glm::abs(pn.eval(glm::vec2(i * 0.1, j * 0.1)) * TILING_HEIGHT))
            );
        }
    }

    // Cube instances
    Cube cube1;

    int NUM_CUBES = 0;
    for (int i = 0; i < TILING_ROWS; i++) {
        for (int k = 0; k < TILING_COLS; k++) {
            int height = tilingHeightmap[TILING_ROWS * i + k];
            if (height == 0.0) {
                cubeTransforms.push_back(
                    glm::translate(
                        glm::mat4(),
                        glm::vec3(
                            static_cast<float>(i),
                            0.0f,
                            static_cast<float>(k)
                        )
                    )
                );
                cubeColors.push_back(WATER);
                NUM_CUBES++;
            }
            else {
                for (int j = 0; j < height; j++) {
                    cubeTransforms.push_back(
                        glm::translate(
                            glm::mat4(),
                            glm::vec3(
                                static_cast<float>(i),
                                static_cast<float>(j),
                                static_cast<float>(k)
                            )
                        )
                    );
                    /*cubePhases[idx] = static_cast<float>(rand()) / RAND_MAX;*/
                    /*cubeColors[NUM_CUBES] = glm::vec3(
                        0.2f,
                        0.2f,
                        j / static_cast<float>(TILING_HEIGHT)
                    );*/
                    float relativeHeight = static_cast<float>(height) / TILING_HEIGHT;
                    if (relativeHeight >= SNOW_LEVEL) {
                        cubeColors.push_back(SNOW);
                    }
                    else if (relativeHeight >= ROCK_LEVEL) {
                        cubeColors.push_back(ROCK);
                    }
                    else if (relativeHeight >= GRASS_LEVEL) {
                        cubeColors.push_back(GRASS);
                    }
                    else if (relativeHeight >= DIRT_LEVEL) {
                        cubeColors.push_back(DIRT);
                    }
                    else {
                        cubeColors.push_back(SAND);
                    }
                    NUM_CUBES++;
                }
            }
        }
    }

    /*for (int i = 0; i < TILING_ROWS; ++i) {
        for (int j = 0; j < TILING_COLS; ++j) {
            int idx = TILING_ROWS * i + j;
            cubeTransforms[idx] =
                glm::translate(glm::mat4(), glm::vec3(static_cast<float>(i), 0.0f, static_cast<float>(j)));
            cubePhases[idx] = static_cast<float>(rand()) / RAND_MAX;
            cubeColors[idx] = glm::vec3(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX
            );
        }
    }*/

    GLuint cubeTransformsVBO;
    glGenBuffers(1, &cubeTransformsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeTransforms.size() * sizeof(glm::mat4), cubeTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint cubeColorsVBO;
    glGenBuffers(1, &cubeColorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeColors.size() * sizeof(glm::mat4), cubeColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create VAOs and VBOs
    GLuint cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);

    // Put stuff in array buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube1.m_vertices), cube1.m_vertices, GL_STATIC_DRAW);

    // Attributing vertices - How I channel cubeVBO inputs into the inputs of the vShader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);

    // Binding cube transform instances
    glBindBuffer(GL_ARRAY_BUFFER, cubeTransformsVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(3 + i, 1);
    }

    // Binding cube color instances
    glBindBuffer(GL_ARRAY_BUFFER, cubeColorsVBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glVertexAttribDivisor(1, 1);

    // Reset binds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // FPS metrics
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int frameCounter = 0;

    // Enable V-sync for v-blanks
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        // We are now around 60 FPS, 16-17 ms per frame.
        currTime = glfwGetTime();
        timeDiff = currTime - prevTime;
        frameCounter++;
        if (timeDiff >= 1.0 / 30.0) {
            std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);
            std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
            std::string windowTitle = "FPS: " + FPS + " | ms: " + ms;
            glfwSetWindowTitle(window, windowTitle.c_str());
            prevTime = currTime;
            frameCounter = 0;
        }

        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        cam.processMovement(window, deltaTime);
        processInput(window);

        // Clear colors on screen so we start at fresh slate
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Transforms of Cube
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = cam.getViewMat();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 proj_view = proj * view;

        // Add transforms as uniforms
        GLuint model_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        GLuint proj_view_loc = glGetUniformLocation(shaderProgram, "proj_view");
        glUniformMatrix4fv(proj_view_loc, 1, GL_FALSE, glm::value_ptr(proj_view));

        // Update sinusoidal transforms of cubes
        /*timeDiff = currTime - PROGRAM_START_TIME;
        for (int i = 0; i < NUM_CUBES; i++) {
            float sin_height = AMPLITUDE * glm::sin(OMEGA * (timeDiff + cubePhases[i]) + cubePhases[i]);
            cubeTransforms[i][3][1] = sin_height;
        }
        glBindBuffer(GL_ARRAY_BUFFER, cubeTransformsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTransforms), cubeTransforms, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);*/

        // Draw
        glUseProgram(shaderProgram);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouseCallback(GLFWwindow* window, double dXPos, double dYPos)
{
    float xpos = static_cast<float>(dXPos);
    float ypos = static_cast<float>(dYPos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    cam.processMouseMovement(xoffset, yoffset);

    lastX = xpos;
    lastY = ypos;
}