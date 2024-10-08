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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include "fps_counter.h"
#include "camera.h"

bool isRotating = true;
bool spacePressed = false;

void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void processInput(GLFWwindow* w);
void processCameraPosition(GLFWwindow* w, float deltaTime, Camera& camera);

GLFWwindow* windowAndVieportInit(int wWidth, int wHeight, int vpXStart, int vpYStart, int vpWidth, int vpHeight);

std::vector<float> getVertices();
std::vector<unsigned int> getIndices();
unsigned int getVBOCube();
unsigned int generateVAO(unsigned int VBO, const std::vector<unsigned int>& indices);

int main() {
    GLFWwindow* window = windowAndVieportInit(800, 600, 0, 0, 800, 600);

    //////////////////// -- VAO -- ///////////////////
    unsigned int cubeVBO = getVBOCube();
    unsigned int VAO = generateVAO(cubeVBO, std::vector<unsigned int> ());
    unsigned int lightVAO = generateVAO(cubeVBO, std::vector<unsigned int> ());

    //before texture vonfigure need to use shader program
    Shader lightingShader(std::string(SHADER_DIR) + "vertex.glsl", std::string(SHADER_DIR) + "fragment.glsl");
    Shader lightSourceShader(std::string(SHADER_DIR) + "lightSourceVertex.glsl", std::string(SHADER_DIR) + "lightSourceFragment.glsl");
    glm::vec3 lightPos(0.0f); //(1.2f, 1.0f, -2.0f);
    glm::vec3 radiusVec(0.0f, 0.0f, -1.0f);
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::vec3 rotationCenterVec(0.0f, 0.0f, -1.0f);
    glm::mat4 modelLighting = glm::mat4(1.0f);
    float angle = 0.0f;

    // lightingShader.use();

    ///////////////////// -- Matrices Generation -- //////////////////////////

    Camera camera;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, Camera::MouseMovementCallback);
    glfwSetScrollCallback(window, Camera::MouseScrollCallback);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), 800.0f/600.0f, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        //Draw lighting source
        lightSourceShader.use();
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));        
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        
        if (isRotating) {
            modelLighting = glm::mat4(1.0f);
            modelLighting = glm::translate(modelLighting, rotationCenterVec);
            modelLighting = glm::rotate(modelLighting, angle, axis);
            angle += 1.0f * deltaTime;
            modelLighting = glm::translate(modelLighting, radiusVec);
            lightPos = glm::vec3(view*modelLighting * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            modelLighting = glm::scale(modelLighting, glm::vec3(0.2f));
        }
        else {
            lightPos = glm::vec3(view * modelLighting * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
            
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(modelLighting));

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw Cube
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        lightingShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);


        glUniformMatrix4fv(glGetUniformLocation(lightingShader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));        
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);

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
    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        isRotating = !isRotating;
        spacePressed = true;
    }
    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spacePressed = false;
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
        // positions            // normals      // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
}
std::vector<unsigned int> getIndices() {
    return std::vector<unsigned int> {
        0, 1, 2,
        2, 3, 0
    };
}

unsigned int getVBOCube() {
    static bool isGenerated = false;
    static unsigned int VBO;
    if (isGenerated) {
        return VBO;
    }

    std::vector<float> vertices = getVertices();

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    isGenerated = true;

    return VBO;
}
unsigned int generateVAO(unsigned int VBO, const std::vector<unsigned int>& indices) {
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (VAO == 1) {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    else {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    if (indices.size() != 0) {
        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*indices.size(), indices.data(), GL_STATIC_DRAW);
    }

    return VAO;
}

