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
#include "camera/Camera.h"
#include "tiling/TilingWorld.h"
#include "shapes/Cube.h"

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
    GLuint shaderProgram = sh.compileShaders("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
    GLuint lightShaderProgram = sh.compileShaders("shaders/LightCubeVertexShader.glsl", "shaders/LightCubeFragmentShader.glsl");

    // Enable z-buffer depth testing
    glEnable(GL_DEPTH_TEST);

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Let's render some stuff!
    std::vector<int> a;
    int tiling_rows = 70;
    int tiling_cols = 70;
    int tiling_height = 50;
    float omega = 10.0f;
    float amplitude = 0.2f;
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(10.0f, 30.0f, 15.0f);
    TilingWorld world(tiling_rows, tiling_cols, tiling_height, omega, amplitude);

    world.generateWorld(2013);

    // Create light
    GLuint lightVAO, lightVBO;
    glGenBuffers(1, &lightVBO);
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Tile::vertices), Tile::vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Add transform uniform to light cube shader.
    sh.setUniformMat4fv(
        lightShaderProgram,
        "transform", 
        glm::value_ptr(glm::translate(glm::mat4(), lightPosition))
    );

    // Add light uniforms to world shader.
    sh.setUniform3fv(shaderProgram, "lightColor", glm::value_ptr(lightColor));
    sh.setUniform3fv(shaderProgram, "lightPosition", glm::value_ptr(lightPosition));

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

        // Move light
        float newLightPosX = 10.0 * glm::cos(currTime);
        float newLightPosZ = 10.0 * glm::sin(currTime);
        lightPosition[0] = newLightPosX + 40.0f;
        lightPosition[2] = newLightPosZ + 15.0f;
        sh.setUniformMat4fv(
            lightShaderProgram,
            "transform",
            glm::value_ptr(glm::translate(glm::mat4(), lightPosition))
        );
        sh.setUniform3fv(shaderProgram, "lightPosition", glm::value_ptr(lightPosition));
        sh.setUniform3fv(shaderProgram, "camPos", glm::value_ptr(cam.m_pos));

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

        // Add transforms as uniforms (World shader program)
        sh.setUniformMat4fv(shaderProgram, "model", glm::value_ptr(model));
        sh.setUniformMat4fv(shaderProgram, "proj_view", glm::value_ptr(proj_view));

        // Add transforms as uniforms (Light shader program)
        sh.setUniformMat4fv(lightShaderProgram, "model", glm::value_ptr(model));
        sh.setUniformMat4fv(lightShaderProgram, "proj_view", glm::value_ptr(proj_view));

        // Draw

        sh.useShaderProgram(shaderProgram);
        world.animateWater(currTime - PROGRAM_START_TIME);
        world.renderTiles();

        sh.useShaderProgram(lightShaderProgram);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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