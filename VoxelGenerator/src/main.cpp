#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glad.h"
#include <vector>

struct Vertex {
    Vertex(float x, float y, float z, float r, float g, float b) {
        position.x = x;
        position.y = y;
        position.z = z;
        color.r = r;
        color.g = g;
        color.b = b;
    }
    glm::vec3 position;
    glm::vec3 color;
};

static GLuint meshVAO;
static GLuint meshVBO;
static std::vector<Vertex> vertices;

void updateVAO();

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

    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);


    updateVAO();

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwSwapBuffers(window);
    }

    return 0;
}

void updateVAO() {
    vertices.clear();

    vertices.emplace_back(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertices.emplace_back(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertices.emplace_back(1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}