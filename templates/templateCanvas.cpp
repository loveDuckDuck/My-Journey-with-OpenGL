#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// custom utils
#include "../util/Callback.h"
#include "../util/TextureLoader.h"
#include "../util/Filesystem.h"
#include "../util/Shader.h"
#include "../util/Camera.h"
#include "../util/Model.h"
#include "../util/Material.h"
#include "../util/CanvasCube.h"

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // Create my sunny beatiful windows
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // define all the callback to my windows, or interfaction with it, wiht I/O modules
    // --------------------------------------------------------------------------------
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    // -------------------------------
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // OPENGL use the coordinate system in the bottom-left position
    // tipically the image start in the top-left position
    // withour flipping the textyure it will be load upsidedown
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // add all monkey to my world
    // --------------------------------

    // add the shader to my monkeys
    // --------------------------------

    // if want i add the texture to my monkeys
    // --------------------------------

    // know i create my canvas, which is the cube
    // ---------------------------------------------
    CanvasCube quadCube;
    quadCube.initCanvas();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameMovement = static_cast<float>(camera.GetSpeedCamera() * deltaTime);

        // input
        // -----
        processInput(window);

        // FIRST PASS: render scene to framebuffer
        // ========================================
        glBindFramebuffer(GL_FRAMEBUFFER, quadCube.getFramebuffer());
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // FIRST PASS: render scene to framebuffer
        // ========================================
        // start painting out off camera
        // initiliziase all the shaders alla the uniform in it
        // ---------------------------------------------------
        // SECOND PASS: now draw framebuffer texture to screen
        // ===================================================
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test for screen-space quad
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        quadCube.useCanvas();

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    quadCube.deleteBuffers();

    // glfw: terminate
    // ---------------
    glfwTerminate();
    return 0;
}