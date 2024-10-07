#ifndef SHADER_DIR
#define SHADER_DIR
#endif
#ifndef TEXTURE_DIR
#define TEXTURE_DIR
#endif

#include <cmath>
#include <iostream>

#include <glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* w) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(w, true);
    }
}

void setOpacity(GLFWwindow* w, const Shader& s) {
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

int main() {
    glfwInit();
    // OPTION - VALUE OF AN OPTION
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD";
        return -1;
    }
    
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //////////////////// -- VAO -- ///////////////////

    float vertices[] = {
        // positions // colors // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.6f, 0.6f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.6f, 0.4f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f, 0.4f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.4f, 0.6f // top left
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float))); // указали лэйаут
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ///////////////////////////// -- texture -- ///////////////////////

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

    ////////////////////// -- 2 texture -- //////////////////////////
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // фильтеринг, есть параметр с mipmap 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::string texture2Path = std::string(TEXTURE_DIR) + "awesomeface.png";
    data = stbi_load(texture2Path.data(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "FAILED TO DOWNLOAD A TEXTURE" << std::endl;
        return -1;
    }
    stbi_image_free(data);

    ///////////////////// -- Shader Program -- ////////////////////

    Shader shader(std::string(SHADER_DIR) + "vertex.glsl", std::string(SHADER_DIR) + "fragment.glsl");
    shader.use();

    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window)) {
        //1. Check Imput
        processInput(window);
        setOpacity(window, shader);
        //2. Call all the rendering commands
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        
        // glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        //3. Check and call events -> swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}