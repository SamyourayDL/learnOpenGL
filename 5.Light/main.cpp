#ifndef SHADER_DIR
#define SHADER_DIR
#endif
#ifndef TEXTURE_DIR
#define TEXTURE_DIR
#endif

#include <cmath>
#include <iostream>
#include <exception>
#include <memory>

#include <glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
// #include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include "fps_counter.h"
#include "camera.h"

// task from learnOpenGL

void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void processInput(GLFWwindow* w);
void processCameraPosition(GLFWwindow* w, float deltaTime, Camera& camera);

GLFWwindow* windowAndVieportInit(int wWidth, int wHeight, int vpXStart, int vpYStart, int vpWidth, int vpHeight);

std::vector<float> getVertices();
std::vector<unsigned int> getIndices();
unsigned int generateVAO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

int main() {
    GLFWwindow* window = windowAndVieportInit(800, 600, 0, 0, 800, 600);

    //////////////////// -- VAO -- ///////////////////
    unsigned int VAO = generateVAO(getVertices(), std::vector<unsigned int> ());

    //before texture vonfigure need to use shader program
    Shader shader(std::string(SHADER_DIR) + "vertex.glsl", std::string(SHADER_DIR) + "fragment.glsl");
    shader.use();

    ///////////////////// -- Matrices Generation -- //////////////////////////

    Camera camera;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, Camera::MouseMovementCallback);
    glfwSetScrollCallback(window, Camera::MouseScrollCallback);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //1. Check Imput
        processInput(window);
        processCameraPosition(window, deltaTime, camera);
        //2. Call all the rendering commands
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        // glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), 800.0f/600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        //3. Check and call events -> swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* w) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(w, true);
    }
}
void processCameraPosition(GLFWwindow* w,  float deltaTime, Camera& camera) {
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    else if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    else if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    else if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }
}


GLFWwindow* windowAndVieportInit(int wWidth, int wHeight, int vpXStart, int vpYStart, int vpWidth, int vpHeight) {
    glfwInit();
    // OPTION - VALUE OF AN OPTION
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window";
        glfwTerminate();
        throw std::runtime_error("windowAndVieportInit()");
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD";
        throw std::runtime_error("windowAndVieportInit()");
    }
    
    glViewport(vpXStart, vpYStart, vpWidth, vpHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

std::vector<float> getVertices() {
    return std::vector<float> {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f, 
        0.5f, 0.5f, -0.5f, 
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f, 
        0.5f, -0.5f, 0.5f, 
        0.5f, 0.5f, 0.5f, 
        0.5f, 0.5f, 0.5f, 
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, 
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f
    };
}
std::vector<unsigned int> getIndices() {
    return std::vector<unsigned int> {
        0, 1, 2,
        2, 3, 0
    };
}

unsigned int generateVAO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    if (indices.size() != 0) {
        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*indices.size(), indices.data(), GL_STATIC_DRAW);
    }

    return VAO;
}

