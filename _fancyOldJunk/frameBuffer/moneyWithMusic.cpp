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
#include "../util/MusicPlayer.h"

int main(int argc, char* argv[])
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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    Model moneyTest(FileSystem::getPath("Test/monkey.obj"));
    // add the shader to my monkeys
    // --------------------------------
    Shader shaderMonkey(FileSystem::getPath("Shaders/material_vertex.vs").c_str(), FileSystem::getPath("Shaders/material_fragment.fs").c_str());
    // if want i add the texture to my monkeys
    // --------------------------------
    Material material = Materials::TURQUOISE;
    // know i create my canvas, which is the cube
    // ---------------------------------------------

    // define some values for the light like
    glm::vec3 ligthConstant = glm::vec3(1.0f);
    float outerCutOffCostant = glm::cos(glm::radians(34.0f));
    float cutOffCostant = glm::cos(glm::radians(25.0f));
    float constantDistance = 1.0f;
    float linearDistance = 0.09f;
    float quadraticDistance = 0.032f;

    CanvasCube quadCube;
    quadCube.initCanvas();


    std::string musicFile;
    
    if (argc > 1) {
        musicFile = argv[1];
    } else {
        std::cout << "Enter music file path (relative): ";
        std::getline(std::cin, musicFile);
    }
    
    if (musicFile.empty()) {
        musicFile = "./music.m4a"; // Default to M4A file
        std::cout << "Using default path: " << musicFile << std::endl;
    }
    
    MusicPlayer player;
    
    if (!player.initialize()) {
        std::cerr << "Failed to initialize OpenAL" << std::endl;
        return -1;
    }
    
    if (!player.loadMusic(musicFile)) {
        std::cerr << "Failed to load music file: " << musicFile << std::endl;
        return -1;
    }
    

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
        //processInput(window);

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

        // define my project o need it to pass from te 
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.f);
        glm::mat4 viewMatrix = camera.GetViewMatrix();


        
        shaderMonkey.use();
        
        player.play();

        
        
        shaderMonkey.setVec3("spotLight.position", camera.Position);
        shaderMonkey.setVec3("spotLight.direction", camera.Front);
        shaderMonkey.setVec3("spotLight.ambient", ligthConstant);
        shaderMonkey.setVec3("spotLight.diffuse", ligthConstant);
        shaderMonkey.setVec3("spotLight.specular", ligthConstant);

        shaderMonkey.setFloat("spotLight.constant", constantDistance);
        shaderMonkey.setFloat("spotLight.linear", linearDistance);
        shaderMonkey.setFloat("spotLight.quadratic", quadraticDistance);
        shaderMonkey.setFloat("spotLight.cutOff", cutOffCostant);
        shaderMonkey.setFloat("spotLight.outerCutOff", outerCutOffCostant);

        // Add after setting spotlight uniforms:
        shaderMonkey.setVec3("material.ambient", material.ambient);
        shaderMonkey.setVec3("material.diffuse", material.diffuse);
        shaderMonkey.setVec3("material.specular", material.specular);
        shaderMonkey.setFloat("material.shininess", material.shininess);


        shaderMonkey.setMat4("projection",projectionMatrix);
        shaderMonkey.setMat4("view",viewMatrix);
        shaderMonkey.setVec3("viewPos",camera.Position);


        // now i need the final matrix that one who move my young boy model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,glm::vec3(0.0f));
        model = glm::scale(model,glm::vec3(1.0f));
        
        shaderMonkey.setMat4("model",model);
        moneyTest.Draw(shaderMonkey);
        
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