#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// custom utils
#include "../util/Callback.h"
#include "../util/Texture.h"
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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader ourShader("4.normal_mapping.vs", "4.normal_mapping.fs");
    Shader rockShader("rock_vertex.vs", "rock_fragment.fs");
    // load models
    // -----------
    Model ourModel(FileSystem::getPath("Test/smooth_monkey.obj"));
    Model cubeModel(FileSystem::getPath("Test/smooth_monkey.obj"));

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // spotlight costant
    // ------------------

    glm::vec3 spotLightAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);


    unsigned int metal_normal = TextureLoader::loadTexture(FileSystem::getPath("metal_plate_nor_gl_1k.jpg").c_str());
    rockShader.setInt("metal_normal", 0);

        unsigned int metal_diff = TextureLoader::loadTexture(FileSystem::getPath("metal_plate_diff_1k.jpg").c_str());
    rockShader.setInt("metal_diff", 1);
        unsigned int metal_spec = TextureLoader::loadTexture(FileSystem::getPath("metal_plate_spec_1k.jpg").c_str());
    rockShader.setInt("metal_spec", 2);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" <<  std::endl;
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
        ourModel.Framebuffer();
        cubeModel.Framebuffer();
        glEnable(GL_STENCIL_TEST);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);

        // set the material
        ourShader.setFloat("material.shininess", 16.0f);
        ourShader.setVec3("material.ambient", glm::vec3(0.0215f, 0.1745f, 0.0215f));
        ourShader.setVec3("material.diffuse", glm::vec3(.07568f, 0.61424f, 0.07568));
        ourShader.setVec3("material.specular", glm::vec3(0.633f, 0.727811f, 0.633f));

        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", spotLightAmbient);
        ourShader.setVec3("spotLight.diffuse", spotLightDiffuse);
        ourShader.setVec3("spotLight.specular", spotLightDiffuse);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(25.0f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(34.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);



        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down

        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // Second model with rock shader and texture
        rockShader.use();
        rockShader.setVec3("viewPos", camera.Position);

        // spotLight
        rockShader.setVec3("spotLight.position", camera.Position);
        rockShader.setVec3("spotLight.direction", camera.Front);
        rockShader.setVec3("spotLight.ambient", spotLightAmbient);
        rockShader.setVec3("spotLight.diffuse", spotLightDiffuse);
        rockShader.setVec3("spotLight.specular", spotLightDiffuse);
        rockShader.setFloat("spotLight.constant", 1.0f);
        rockShader.setFloat("spotLight.linear", 0.09f);
        rockShader.setFloat("spotLight.quadratic", 0.032f);
        rockShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(25.0f)));
        rockShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(34.0f)));



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metal_normal);
        rockShader.setInt("metal_normal", 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metal_diff);
        rockShader.setInt("metal_diff", 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metal_spec);
        rockShader.setInt("metal_spec", 2);
        
        rockShader.setMat4("projection", projection);
        rockShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down

        rockShader.setMat4("model", model);

        cubeModel.Draw(rockShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
