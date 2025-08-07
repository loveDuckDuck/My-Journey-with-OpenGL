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

    // TODO fix issue mouse
    // tell GLFW to capture our mouse
    // glfwSetMouseButtonCallback(window, mouseButtonCallback);
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
    // this is effcienet because avoid the calling of fragmente shaders
    // for all the intern faces, so we need to call it nonly the 50 % of the timne
    // it gona take only clockwise vertix, using fron view
    // GL_BACK: Culls only the back faces.
    // GL_FRONT: Culls only the front faces.
    // GL_FRONT_AND_BACK: Culls both the front and back faces.
    // -----------------------------------------------------------------
    glEnable(GL_CULL_FACE);
    // build and compile shaders
    // -------------------------
    Shader ourShader("framebuffers.vs", "framebuffers.fs");
    Shader monkeyTestShader("framebuffers.vs", "framebuffers.fs");

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("Test/monkey.obj"));
    Model monkeyTest2(FileSystem::getPath("Test/monkey.obj"));

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    unsigned int rockColor = TextureLoader::loadTexture(FileSystem::getPath("Rock_Color.jpg").c_str());
    ourShader.use();
    ourShader.setInt("texture1", 0);

    unsigned int metal = TextureLoader::loadTexture(FileSystem::getPath("metal_plate_diff_1k.jpg").c_str());
    monkeyTestShader.use();
    monkeyTestShader.setInt("texture1", 0);

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
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);           // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        for (Mesh mesh : ourModel.getMeshes())
        {
            glBindVertexArray(mesh.getVAO());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rockColor);
        }

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);
        // glClear(GL_COLOR_BUFFER_BIT);
        //  glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //  -------------------------------------------------------------------------------

        // now define the other money
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear all relevant buffers
        glEnable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT);
        monkeyTestShader.use();

        monkeyTestShader.setVec3("viewPos", camera.Position);
        // view/projection transformations

        monkeyTestShader.setMat4("projection", projection);
        monkeyTestShader.setMat4("view", view);

        for (Mesh mesh : monkeyTest2.getMeshes())
        {
            glBindVertexArray(mesh.getVAO());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, metal);
        }

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        monkeyTestShader.setMat4("model", model);

        monkeyTest2.Draw(monkeyTestShader);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteFramebuffers(1, &framebuffer);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
