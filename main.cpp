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
#include "tiling/TilingWorld.h"

// About tiling
//const int NUM_CUBES = 100;
//const int TILING_ROWS = 70;
//const int TILING_COLS = 70;
//const int TILING_HEIGHT = 40;
//const float OMEGA = 5.0f;
//const float AMPLITUDE = 0.2f;

// Each terrain type is associated with 
// - Elevation
// - Color
//const float WATER_LEVEL = 0.0f;
//const float SAND_LEVEL  = 0.0f;
//const float DIRT_LEVEL  = 0.2f;
//const float GRASS_LEVEL = 0.4f;
//const float ROCK_LEVEL  = 0.6f;
//const float SNOW_LEVEL  = 0.8f;
//const glm::vec3 WATER = glm::vec3(0.0f, 0.0f, 150.0f / 255.0f);
//const glm::vec3 SAND = glm::vec3(237.0f / 255.0f, 201.0f / 255.0f, 175.0f / 255.0f);
//const glm::vec3 DIRT = glm::vec3(155.0f / 255.0f, 118.0f / 255.0f, 83.0f / 255.0f);
//const glm::vec3 GRASS = glm::vec3(34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f);
//const glm::vec3 ROCK = glm::vec3(120.0f / 255.0f, 120.0f / 255.0f, 120.0f / 255.0f);
//const glm::vec3 SNOW = glm::vec3(1.0f, 1.0f, 1.0f);

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
    std::vector<int> a;
    int tiling_rows = 70;
    int tiling_cols = 70;
    int tiling_height = 40;
    float omega = 5.0f;
    float amplitude = 0.2f;
    TilingWorld world(tiling_rows, tiling_cols, tiling_height, omega, amplitude);

    world.generateWorld(20);

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

    // FPS metrics
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int frameCounter = 0;

    // Enable V-sync for v-blanks
    glfwSwapInterval(1);

    std::cout << "Starting to render" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        // we are now around 60 fps, 16-17 ms per frame.
        currTime = glfwGetTime();
        timeDiff = currTime - prevTime;
        frameCounter++;
        if (timeDiff >= 1.0 / 30.0) {
            std::string fps = std::to_string((1.0 / timeDiff) * frameCounter);
            std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
            std::string windowtitle = "fps: " + fps + " | ms: " + ms;
            glfwSetWindowTitle(window, windowtitle.c_str());
            prevTime = currTime;
            frameCounter = 0;
        }

        float currentframe = (float)glfwGetTime();
        deltaTime = currentframe - lastFrame;
        lastFrame = currentframe;

        cam.processMovement(window, deltaTime);
        processInput(window);

        // clear colors on screen so we start at fresh slate
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // transforms of cube
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = cam.getViewMat();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 proj_view = proj * view;

        // add transforms as uniforms
        GLuint model_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        GLuint proj_view_loc = glGetUniformLocation(shaderProgram, "proj_view");
        glUniformMatrix4fv(proj_view_loc, 1, GL_FALSE, glm::value_ptr(proj_view));

        // update sinusoidal transforms of cubes
        /*timediff = currtime - program_start_time;
        for (int i = 0; i < num_cubes; i++) {
            float sin_height = amplitude * glm::sin(omega * (timediff + cubephases[i]) + cubephases[i]);
            cubetransforms[i][3][1] = sin_height;
        }
        glbindbuffer(gl_array_buffer, cubetransformsvbo);
        glbufferdata(gl_array_buffer, sizeof(cubetransforms), cubetransforms, gl_static_draw);
        glbindbuffer(gl_array_buffer, 0);
        glbindvertexarray(0);*/

        // draw
        glUseProgram(shaderProgram);
        /*glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);*/
        world.renderTiles();

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