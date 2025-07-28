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
#include "light/Light.h"

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
    GLuint shadowMapProgram = sh.compileShaders("shaders/ShadowMapVertexShader.glsl", "shaders/ShadowMapFragmentShader.glsl");

    // Enable z-buffer depth testing
    glEnable(GL_DEPTH_TEST);

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Let's render some stuff!
    int tiling_rows = 70;
    int tiling_cols = 70;
    int tiling_height = 50;
    float omega = 10.0f;
    float amplitude = 0.2f;
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(0.0f, 40.0f, 0.0f);
    TilingWorld world(tiling_rows, tiling_cols, tiling_height, omega, amplitude);
    world.initLight(lightColor, lightPosition);

    // Depth testing and shadow mapping
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // Create a 2D texture to write to. We're going to attach this
    // to our framebuffer.
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // We don't want color to be rendered.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    world.generateWorld(2026);

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

    // Enable early z depth testing
    //glEnable(GL_DEPTH_TEST);

    // Culling back faces
    glEnable(GL_CULL_FACE);

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
        glm::vec3 newLightPosition = world.animateLight();
        sh.setUniformMat4fv(
            lightShaderProgram,
            "transform",
            glm::value_ptr(glm::translate(glm::mat4(), newLightPosition))
        );
        sh.setUniform3fv(shaderProgram, "lightPosition", glm::value_ptr(newLightPosition));
        sh.setUniform3fv(shaderProgram, "camPos", glm::value_ptr(cam.m_pos));

        float currentframe = (float)glfwGetTime();
        deltaTime = currentframe - lastFrame;
        lastFrame = currentframe;

        cam.processMovement(window, deltaTime);
        processInput(window);

        // clear colors on screen so we start at fresh slate
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // First pass: we render to our directional light source
        float near_plane = 1.0f, far_plane = 600.0f;
        glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(
            newLightPosition,
            glm::vec3(70.0f, 0.0f, 70.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpace_proj_view = lightProjection * lightView; // World -> View -> Projection (Clip)

        // transforms of cube
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = cam.getViewMat();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, near_plane, far_plane);
        glm::mat4 proj_view = proj * view;

        // Set uniforms for first pass render with respect to light directional light.
        sh.setUniformMat4fv(shadowMapProgram, "model", glm::value_ptr(model));
        sh.setUniformMat4fv(shadowMapProgram, "proj_view", glm::value_ptr(lightSpace_proj_view));

        // Add transforms as uniforms (World shader program)
        sh.setUniformMat4fv(shaderProgram, "model", glm::value_ptr(model));
        sh.setUniformMat4fv(shaderProgram, "proj_view", glm::value_ptr(proj_view));
        sh.setUniformMat4fv(shaderProgram, "lightSpace_proj_view", glm::value_ptr(lightSpace_proj_view));

        // Add transforms as uniforms (Light shader program)
        sh.setUniformMat4fv(lightShaderProgram, "model", glm::value_ptr(model));
        sh.setUniformMat4fv(lightShaderProgram, "proj_view", glm::value_ptr(proj_view));

        // First Pass: For light
        glCullFace(GL_FRONT);
        sh.useShaderProgram(shadowMapProgram);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw
        world.animateWater(currTime - PROGRAM_START_TIME);
        world.renderTiles();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 800, 600);
        glCullFace(GL_BACK);

        // Draw
        sh.useShaderProgram(shaderProgram);
        world.animateWater(currTime - PROGRAM_START_TIME);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        world.renderTiles();

        sh.useShaderProgram(lightShaderProgram);
        //world.renderLight();

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

// Some notes on framebuffers and shadow maps
//
// Framebuffers are what you want to draw, so you have to bind either a
// color, depth, or stencil buffer to them.
// 
// We want to render stuff off-screen (hence another framebuffer), in which
// if you attach a depth buffer to this off-screen framebuffer, the depth would
// be written to this framebuffer, which is our texture, depth buffer. We
// then use this depth buffer, which is a texture based on depth, as a texture
// in the vertex and fragment shader. We then uniform sample2d of this texture
// at certain tex-coords and we good.
// 
// Another thing is we have to make sure it can still do depth testing. We usually
// make an RBO for it because it usually doesn't need to be sampled and it is used
// behind the scenes for OpenGL to do depth testing. 
// 
// BASICALLY: For a framebuffer to be complete, it has to have a color buffer, and
// it can optionally have depth and stencil attachments. In the framebuffer tutorial, 
// the color buffer was a texture, cuz we need to sample it later. The depth and stencil
// were in an RenderBufferObject (RBO), because we don't need to sample from it later.
// At render time, per fragment, shit is rendered into this bound framebuffer (which
// includes all the color, depth, and stencil buffers). It puts fragcolor to the color
// buffer, and if it does depth testing, it will throw the depth stuff into the depth
// buffer.
// 
// You do one pass for rendering into the offline framebuffer. Then a second pass
// to sample whatever you want. In terms of shadow maps, you have one pass to render
// into one framebuffer with just depth (specifying color as NULL), then another pass
// into the default framebuffer, sampling the depth.
//