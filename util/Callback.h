#ifndef CALLBACK_H
#define CALLBACK_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
// settings
// -------------------------------------------------------
namespace callback
{
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    // define the callback
    // -------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void processInput(GLFWwindow *window);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
}

#endif