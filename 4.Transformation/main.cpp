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
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "fps_counter.h"

float personHeight = 2.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, personHeight, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float pitch = 0.0;
float yaw = -90.0;
bool firstMouse = true;
float fov = 45.0f;

// task from learnOpenGL
glm::mat4 lookAt(glm::vec3 cameraPosition, glm::vec3 target, glm::vec3 Up) {
    glm::vec3 zPlus = glm::normalize(cameraPosition - target);
    glm::vec3 xPlus = glm::cross(Up, zPlus);
    glm::vec3 yPlus = glm::cross(zPlus, xPlus);
    glm::vec4 one(0.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 rot(glm::vec4(xPlus, 0.0f), glm::vec4(yPlus, 0.0f), glm::vec4(zPlus, 0.0f), one);
    rot = glm::transpose(rot);

    glm::mat4 LookAt(1.0f);
    LookAt[3] = glm::vec4(-cameraPosition, 1.0f);
    LookAt = rot * LookAt;

    return LookAt;
}

void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void processInput(GLFWwindow* w);
void processCameraPosition(GLFWwindow* w, float deltaTime);
void processOpacity(GLFWwindow* w, const Shader& s);
void mouse_input_callback(GLFWwindow* w, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


GLFWwindow* windowAndVieportInit(int wWidth, int wHeight, int vpXStart, int vpYStart, int vpWidth, int vpHeight);

std::vector<float> getVertices();
std::vector<unsigned int> getIndices();
unsigned int generateVAO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

unsigned int firstTextureLoadAndConfigure();
unsigned int secondTextureLoadAndConfigure();

int main() {
    GLFWwindow* window = windowAndVieportInit(800, 600, 0, 0, 800, 600);

    //////////////////// -- VAO -- ///////////////////
    unsigned int VAO = generateVAO(getVertices(), std::vector<unsigned int> ());

    //before texture vonfigure need to use shader program
    Shader shader(std::string(SHADER_DIR) + "vertex.glsl", std::string(SHADER_DIR) + "fragment.glsl");
    shader.use();

    ///////////////////////////// -- texture -- ///////////////////////

    unsigned int texture = firstTextureLoadAndConfigure();
    unsigned int texture2 = secondTextureLoadAndConfigure();
    ///////////////////// -- Shader Program -- ////////////////////

    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    ///////////////////// -- Matrices Generation -- //////////////////////////
    //Model Matrices - generate for every model every iter
     glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };
    //View Matrix - generate every iter, we need to process input
    
    //Projection Matrix = generate one time, can set outside of a loop


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_input_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //1. Check Imput
        processInput(window);
        processCameraPosition(window, deltaTime);
        processOpacity(window, shader);
        //2. Call all the rendering commands
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        // glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), 800.0f/600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        float pw = 1;
        for (int i = 0; i < 10; ++i) {
            pw *= -1;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, (float)glfwGetTime() * 1 / 2.0f *pw + glm::radians(50.0f), glm::vec3(0.5f, i / 10.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
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
void processCameraPosition(GLFWwindow* w,  float deltaTime) {
    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    else if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    else if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    else if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (cameraPos.y < personHeight) {
        cameraPos.y = personHeight;
    }
}
void processOpacity(GLFWwindow* w, const Shader& s) {
    static float opacity = 0.2;
    static bool pressedUpLastFrame = false; 
    static bool pressedDownLastFrame = false;

    if (glfwGetKey(w, GLFW_KEY_UP) == GLFW_PRESS) {
        if (pressedUpLastFrame) {
            pressedUpLastFrame = false;
            return;
        }
        pressedUpLastFrame = true;
        opacity += 0.1f;
        opacity = std::min(1.0f, opacity);
    } else if (glfwGetKey(w, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (pressedDownLastFrame) {
            pressedDownLastFrame = false;
            return;
        }
        pressedDownLastFrame = true;
        opacity -= 0.1f;
        opacity = std::max(0.0f, opacity);
    }
    s.setFloat("opacity", opacity);
}
// double lastX = 400;
// double lastY = 300;
void mouse_input_callback(GLFWwindow* w, double xPos, double yPos) {
    static float lastX = 400;
    static float lastY = 300;
    if (firstMouse) {
        firstMouse = false;
        lastX = xPos;
        lastY = yPos;
    }

    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;  // инвертируем чтобы движения мыши вверх были "позитивными"
    lastX = xPos;
    lastY = yPos;

    const float sensitivity = 0.1f;  // более высокое значение чувствительности
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    else if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    // Центрируем курсор
    // glfwSetCursorPos(w, 400, 300);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
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
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float))); // указали лэйаут
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    if (indices.size() != 0) {
        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*indices.size(), indices.data(), GL_STATIC_DRAW);
    }

    return VAO;
}

unsigned int firstTextureLoadAndConfigure() {
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0); // activate texture unit
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // фильтеринг, есть параметр с mipmap 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0, height = 0, nrChannels = 0;
    std::string texturePath = std::string(TEXTURE_DIR) + "container.jpg";
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texturePath.data(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "FAILED TO DOWNLOAD A TEXTURE" << std::endl;
        return -1;
    }
    stbi_image_free(data);

    return texture;
}
unsigned int secondTextureLoadAndConfigure() {
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // фильтеринг, есть параметр с mipmap 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0, height = 0, nrChannels = 0;
    std::string texture2Path = std::string(TEXTURE_DIR) + "awesomeface.png";
    unsigned char* data = stbi_load(texture2Path.data(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "FAILED TO DOWNLOAD A TEXTURE" << std::endl;
        return -1;
    }
    stbi_image_free(data);

    return texture2;
}



