#ifndef CALLBACK_H
#define CALLBACK_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "UtilDimension.h"
#include <iostream>
#include <format>
#include "Camera.h"
// define the callback
// -------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
// settings
// -------------------------------------------------------

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float currentFrame;
float frameMovement;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;


glm::vec3 screenToWorld(double mouseX, double mouseY)
{
    // Convert mouse coordinates to normalized device coordinates (NDC)
    float x = (2.0f * mouseX) / SCR_WIDTH - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / SCR_HEIGHT;
    float z = 1.0f; // Near plane
    
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    
    // Convert to homogeneous clip coordinates
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
    
    // Convert to eye coordinates
    glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    
    // Convert to world coordinates
    glm::vec3 ray_world = glm::vec3(glm::inverse(viewMatrix) * ray_eye);
    ray_world = glm::normalize(ray_world);
    
    return ray_world;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {    // Convert screen coordinates to world coordinates
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get current mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Convert to world coordinates
        glm::vec3 worldPos = screenToWorld(xpos, ypos);
        
        printf("Clicked at world coordinates: (%.2f, %.2f, %.2f)\n", 
               worldPos.x, worldPos.y, worldPos.z);
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec3 getRotatedPosition(float angle, float radius, glm::vec3 center)
{
    glm::vec3 pos;
    pos.x = center.x + radius * cos(angle);
    pos.z = center.z + radius * sin(angle);
    pos.y = center.y; // same height
    return pos;
}

#endif