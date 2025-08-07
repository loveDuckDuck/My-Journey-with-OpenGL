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
    Shader ourShader("4.normal_mapping.vs", "4.normal_mapping.fs");
    Shader grassShader("blending.vs", "blending.fs");
    // load models
    // -----------
    Model ourModel(FileSystem::getPath("Test/monkey.obj"));
    Model cubeModel(FileSystem::getPath("cube.obj"));

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // spotlight costant
    // ------------------

    glm::vec3 spotLightAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);

    // try to add texture the cubes
    unsigned int transparentTexture = TextureLoader::loadTexture(FileSystem::getPath("grass.png").c_str());
    grassShader.setInt("texture1", 0);


    unsigned int rock_color = TextureLoader::loadTexture(FileSystem::getPath("Rock_Color.jpg").c_str());
    ourShader.setInt("rock_color", 0);

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

        // set the rock color texture
           glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock_color);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        grassShader.setMat4("projection", projection);
        grassShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down

        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        grassShader.setMat4("model", model);
        cubeModel.Draw(grassShader);

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
