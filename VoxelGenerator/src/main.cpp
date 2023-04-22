#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glad.h"

int main() {
    if(!glfwInit()){
        std::cout << "Failed to initialize glfw" << std::endl;
    }
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Window", nullptr, nullptr);    
    if(!window) {
        std::cout << "Failed to create window" << std::endl;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGL()) {
        std::cout << "Failed to initialize glad" << std::endl;
    }

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        

        glfwSwapBuffers(window);
    }



    return 0;
}
