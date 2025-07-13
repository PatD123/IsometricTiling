#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/ShaderHelper.h"
#include "shapes/Cube.h"
#include "camera/Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Compile and link shaders
    ShaderHelper sh;
    GLuint shaderProgram = sh.compileShaders("VertexShader.glsl", "FragmentShader.glsl");

    glUseProgram(shaderProgram);

    // Enable z-buffer depth testing
    glEnable(GL_DEPTH_TEST);

    // Sidenote on instancing
    /*
    glDrawArraysInstanced (or whatever it is called), you specify what your instance is, which 
    if we want to draw cubes is 36 vertices, so that is now considered an "instance". We then
    specify numInstances, which is self-explanatory. The VertexDiv function tells OpenGL how
    often I should be switching to a new Matrix to transform our cube. We want it so that 
    for each instance of a cube, we use a new matrix. That's what it does.
    */

    // Making Camera
    Camera cam(glm::vec3(0.0f, 0.0f, 0.0f));
    const float radius = 5.0f;

    // Cube instances
    Cube cube1;

    // Instancing
    glm::mat4 cubeTransforms[2];
    cubeTransforms[0] = glm::mat4();
    cubeTransforms[1] = glm::translate(glm::mat4(), glm::vec3(2.0f, 2.0f, 0.0f));

    GLuint cubeTransformsVBO;
    glGenBuffers(1, &cubeTransformsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeTransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTransforms), cubeTransforms, GL_STATIC_DRAW);
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

    // Binding transform instance
    glBindBuffer(GL_ARRAY_BUFFER, cubeTransformsVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(3 + i, 1);
    }

    // Reset binds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Clear colors on screen so we start at fresh slate
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::vec3 newCameraPos = glm::vec3(camX, 0.0, camZ);
        cam.setPosition(newCameraPos);

        // Look at
        glm::mat4 lookAtMat = glm::lookAt(
            cam.m_pos, 
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0)
        );

        // Transforms of Cube
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -8.0f));
        view = lookAtMat;
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Add transforms as uniforms
        GLuint model_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        GLuint view_loc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        GLuint proj_loc = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));


        // Draw
        glUseProgram(shaderProgram);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cubeTransforms->length());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
