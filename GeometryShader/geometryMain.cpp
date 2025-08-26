#include "../Umbrella.h"

int main(int argc, char *argv[])
{
    // Initialize GLFW and set context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Geometry Shader", NULL, NULL);
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
    glfwSetKeyCallback(window, keyCallbackResetCamera);

    // Capture mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Create models and shaders
    auto monkeyModel = std::make_unique<Model>(FileSystem::getPath("obj/monkey.obj"));
    auto shaderMonkey = std::make_unique<Shader>(FileSystem::getPath("Shaders/material_vertex.vs").c_str(),
                                                 FileSystem::getPath("Shaders/material_fragment.fs").c_str());

    auto monkeyTexture = std::make_unique<Model>(FileSystem::getPath("obj/zelda.obj"));
    auto shaderTextureMonkey = std::make_unique<Shader>(FileSystem::getPath("Shaders/texture_vertex.vs").c_str(),
                                                        FileSystem::getPath("Shaders/texture_fragment.fs").c_str());

    auto canvas = std::make_unique<CanvasCube>();
    canvas->initCanvas();

    Material material = Materials::COPPER;

    // Spotlight configuration
    glm::vec3 spotLightAmbient = glm::vec3(0.1f, 0.1f, 0.1f); // Reduced ambient
    glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f; // Fixed missing 'f'

    float cutOff = glm::cos(glm::radians(25.0f));
    float outerCutOff = glm::cos(glm::radians(34.0f));

    unsigned int texture = TextureLoader::loadTexture(FileSystem::getPath("Texture/text.jpg").c_str());
    shaderTextureMonkey->use();
    shaderTextureMonkey->setInt("texture1", 0);
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // FIRST PASS: render scene to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, canvas->getFramebuffer());
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader and set uniforms
        shaderMonkey->use();

        // Set spotlight properties
        shaderMonkey->setVec3("spotLight.ambient", spotLightAmbient);
        shaderMonkey->setVec3("spotLight.diffuse", spotLightDiffuse);
        shaderMonkey->setVec3("spotLight.specular", spotLightSpecular);

        shaderMonkey->setFloat("spotLight.cutOff", cutOff);
        shaderMonkey->setFloat("spotLight.outerCutOff", outerCutOff);
        shaderMonkey->setFloat("spotLight.constant", constant);
        shaderMonkey->setFloat("spotLight.linear", linear);
        shaderMonkey->setFloat("spotLight.quadratic", quadratic);

        shaderMonkey->setVec3("spotLight.position", camera.Position);
        shaderMonkey->setVec3("spotLight.direction", camera.Front);

        // Set material properties
        shaderMonkey->setFloat("material.shininess", material.shininess);
        shaderMonkey->setVec3("material.ambient", material.ambient);
        shaderMonkey->setVec3("material.diffuse", material.diffuse);
        shaderMonkey->setVec3("material.specular", material.specular);
        shaderMonkey->setVec3("colorObj", material.color);

        // Set matrices
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        shaderMonkey->setVec3("viewPos", camera.Position);
        shaderMonkey->setMat4("view", view);
        shaderMonkey->setMat4("projection", projection);
        shaderMonkey->setMat4("model", model);

        // Draw the monkey model
        monkeyModel->Draw(*shaderMonkey);

        // add the second money

        shaderTextureMonkey->use();
        for (Mesh mesh : monkeyTexture.get()->getMeshes())
        {
            glBindVertexArray(mesh.getVAO());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        shaderMonkey->setVec3("viewPos", camera.Position);
        shaderMonkey->setMat4("view", view);
        shaderMonkey->setMat4("projection", projection);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model,glm::vec3(3));
        shaderMonkey->setMat4("model", model);

        monkeyTexture->Draw(*shaderMonkey);
        // SECOND PASS: Render framebuffer to screen quad
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // Disable depth test for screen-space quad

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        canvas->useCanvas();

        // Re-enable depth test for next frame
        glEnable(GL_DEPTH_TEST);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    canvas->deleteBuffers();
    glfwTerminate();
    return 0;
}