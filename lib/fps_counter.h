#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

#include <../glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class FPSCounter {
public:
    FPSCounter() {

    }

    void start();
    void update();
    void show() const;
private:
    double startTime;
    unsigned int frames;
    double fps;
};

void FPSCounter::start() {
    startTime = glfwGetTime();
}
void FPSCounter::update() {
    ++frames;
    double finishTime = glfwGetTime();
    if (finishTime - startTime < 1.0) {
        return;
    }
    fps = frames / (finishTime - startTime);
    startTime = finishTime;
    frames = 0;
    show();
}
void FPSCounter::show() const {
    std::cout << fps << std::endl;
};

#endif