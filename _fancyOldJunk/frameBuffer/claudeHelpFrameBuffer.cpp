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
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // build and compile shaders
    // -------------------------
    Shader ourShader("framebuffers.vs", "framebuffers.fs");
    Shader monkeyTestShader("framebuffers.vs", "framebuffers.fs");
    Shader shaderMaterialMonkey("spotLightJadeMokey.vs", "spotLightJadeMokey.fs");

    Shader screenShader("screen.vs", "screen.fs"); // NEW: Screen-space quad shader

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("Test/monkey.obj"));
    Model monkeyTest2(FileSystem::getPath("Test/monkey.obj"));
    Model monkeyJade(FileSystem::getPath("Test/monkey.obj"));
    Model monekySmooth(FileSystem::getPath("Test/smooth_monkey.obj"));

    unsigned int rockColor = TextureLoader::loadTexture(FileSystem::getPath("Rock_Color.jpg").c_str());
    ourShader.use();
    ourShader.setInt("texture1", 0);

    unsigned int metal = TextureLoader::loadTexture(FileSystem::getPath("metal_plate_diff_1k.jpg").c_str());
    monkeyTestShader.use();
    monkeyTestShader.setInt("texture1", 0);

    // Screen quad setup
    // -----------------
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // create a renderbuffer object for depth and stencil attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure screen shader
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // before the loop im gone insert all  the variable i need in my
    // moneky jade shader
    // -----------------------------------------------------------------

    glm::vec3 spotLightAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    Material material = Materials::EMERALD;


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
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render first model
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        for (Mesh mesh : ourModel.getMeshes())
        {
            glBindVertexArray(mesh.getVAO());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rockColor);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // render second model (to same framebuffer)
        monkeyTestShader.use();
        monkeyTestShader.setVec3("viewPos", camera.Position);
        monkeyTestShader.setMat4("projection", projection);
        monkeyTestShader.setMat4("view", view);

        for (Mesh mesh : monkeyTest2.getMeshes())
        {
            glBindVertexArray(mesh.getVAO());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, metal);
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        monkeyTestShader.setMat4("model", model);
        monkeyTest2.Draw(monkeyTestShader);



        // define the third model and set it
        
        
        shaderMaterialMonkey.use();
        // set the material
        shaderMaterialMonkey.setFloat("material.shininess", material.shininess);
        shaderMaterialMonkey.setVec3("material.ambient",material.ambient);
        shaderMaterialMonkey.setVec3("material.diffuse", material.diffuse);
        shaderMaterialMonkey.setVec3("material.specular", material.specular);
        shaderMaterialMonkey.setVec3("colorObj", material.color);


        // define the light of the spotlight
        shaderMaterialMonkey.setVec3("spotLight.ambient", spotLightAmbient);
        shaderMaterialMonkey.setVec3("spotLight.diffuse", spotLightSpecular);
        shaderMaterialMonkey.setVec3("spotLight.specular", spotLightDiffuse);

        // define the value that define my cone of light
        shaderMaterialMonkey.setFloat("spotLight.constant", 1.0);
        shaderMaterialMonkey.setFloat("spotLight.linear", 0.09f);
        shaderMaterialMonkey.setFloat("spotLight.quadratic", 0.032f);
        shaderMaterialMonkey.setFloat("spotLight.cutOff", glm::cos(glm::radians(25.0f)));
        shaderMaterialMonkey.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(34.0f)));
        
        shaderMaterialMonkey.setVec3("spotLight.position", camera.Position);
        shaderMaterialMonkey.setVec3("spotLight.direction", camera.Front);

        shaderMaterialMonkey.setVec3("viewPos",camera.Position);
        shaderMaterialMonkey.setMat4("projection",projection);
        shaderMaterialMonkey.setMat4("view",view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        
        
        shaderMaterialMonkey.setMat4("model", model);
        monkeyJade.Draw(shaderMaterialMonkey);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shaderMaterialMonkey.setMat4("model", model);

        monekySmooth.Draw(shaderMaterialMonkey);




        // SECOND PASS: now draw framebuffer texture to screen
        // ===================================================
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test for screen-space quad
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(1, &framebuffer);

    // glfw: terminate
    // ---------------
    glfwTerminate();
    return 0;
}