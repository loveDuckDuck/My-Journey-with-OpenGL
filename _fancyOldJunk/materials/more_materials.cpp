#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>

#include "../util/Random.h"
#include "../util/Texture.h"
#include "../util/Shader.h"
#include "../util/Cube.h"
#include "../util/Camera.h"

// define the callback
// -------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

glm::vec3 getRotatedPosition(float angle, float radius, glm::vec3 center)
{
    glm::vec3 pos;
    pos.x = center.x + radius * cos(angle);
    pos.z = center.z + radius * sin(angle);
    pos.y = center.y; // same height
    return pos;
}


// settings
// -------------------------------------------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float currentFrame;
float frameMovement;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

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
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader ligthShader("light_shader.vs", "light_fragment.fs");
    Shader material_shader("material_shader.vs", "material_fragment.fs");
    Shader emerald_shader("material_shader.vs", "emerald_fragment.fs");

    Cube cube;
    unsigned int VAOEmerals, VBO;
    glGenVertexArrays(1, &VAOEmerals);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAOEmerals);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cube.getVertexDataSize(), cube.getVertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);




    // the nwe cube doesnt new
    unsigned int VAOCube;
    glGenVertexArrays(1, &VAOCube);
    glBindVertexArray(VAOCube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // the nwe cube doesnt new
    unsigned int VAOLightCube;
    glGenVertexArrays(1, &VAOLightCube);
    glBindVertexArray(VAOLightCube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // lighting
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    // render loop
    // -------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameMovement = static_cast<float>(camera.GetSpeedCamera() * deltaTime);


        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        material_shader.use();
        material_shader.setVec3("objectColor", objectColor);
        material_shader.setVec3("lightColor", lightColor);
    
        material_shader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        material_shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        material_shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        material_shader.setFloat("material.shininess", 32.0f);


        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        material_shader.setVec3("light.ambient", ambientColor);
        material_shader.setVec3("light.diffuse", diffuseColor);
        
        material_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        glm::mat4 view = camera.GetViewMatrix();
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        material_shader.setMat4("model", model);
        model = glm::scale(model, glm::vec3(0.6f)); // a smaller cube

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f,
                                      100.0f);

        material_shader.setUniformTransformation("model", model);
        material_shader.setUniformTransformation("view", camera.GetViewMatrix());
        material_shader.setUniformTransformation("projection", projection);
        material_shader.setVec3("lightPos", lightPos);
        material_shader.setVec3("viewPos", camera.Position);

        // draw our first triangle
        glBindVertexArray(VAOCube);
        glDrawArrays(GL_TRIANGLES, 0, 36);




        emerald_shader.use();
        emerald_shader.setVec3("objectColor", glm::vec3(0.0f,1.0f,0.0f));
        emerald_shader.setVec3("lightColor", lightColor);
    
        emerald_shader.setVec3("material.ambient", glm::vec3(0.0215f, 0.1745f, 0.0215f));
        emerald_shader.setVec3("material.diffuse", glm::vec3(.07568f, 0.61424f, 0.07568));
        emerald_shader.setVec3("material.specular", glm::vec3(0.633f, 0.727811f, 0.633f));
        emerald_shader.setFloat("material.shininess", 0.6f);

        emerald_shader.setVec3("light.ambient", ambientColor);
        emerald_shader.setVec3("light.diffuse", diffuseColor);
        
        emerald_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        
        
    
        material_shader.setMat4("model", model);
        model = glm::translate(model,glm::vec3(1.0f,1.0f,1.0f));
        model = glm::scale(model, glm::vec3(0.6f)); // a smaller cube


        material_shader.setUniformTransformation("model", model);
        material_shader.setUniformTransformation("view", camera.GetViewMatrix());
        material_shader.setUniformTransformation("projection", projection);
        material_shader.setVec3("lightPos", lightPos);
        material_shader.setVec3("viewPos", camera.Position);


        // draw our first triangle
        glBindVertexArray(VAOEmerals);
        glDrawArrays(GL_TRIANGLES, 0, 36);

            // also draw the lamp object
        ligthShader.use();
        ligthShader.setVec3("aColor", glm::vec3(lightColor));
        ligthShader.setMat4("projection", projection);
        ligthShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, getRotatedPosition((float)glfwGetTime(), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
        // model = glm::translate(model,lightPos);
        lightPos = getRotatedPosition((float)glfwGetTime(), 1.0f, lightPos);
        lightPos = lightPos * 0.4f;

        model = glm::scale(model, glm::vec3(0.3f)); // a smaller cube
        ligthShader.setMat4("model", model);

        glBindVertexArray(VAOLightCube);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAOLightCube);
    glDeleteVertexArrays(1, &VAOCube);

    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}