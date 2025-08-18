#include "../Utilities/Umbrella.h"

int main(int argc, char *argv[])
{

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
    glfwSetKeyCallback(window, keyCallbackResetCamera);
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

    // add all monkey to my world
    // --------------------------------
    auto monkey = std::make_unique<Model>(FileSystem::getPath("obj/cube.obj"));
    // add the shader to my monkeys
    // --------------------------------

    auto shaderMonkey = std::make_unique<Shader>(FileSystem::getPath("cubemaps/reflect.vs").c_str(),
                                                 FileSystem::getPath("cubemaps/reflect.fs").c_str());

    auto skybox = std::make_unique<Skybox>();
    // know i create my canvas, which is the cube
    // ---------------------------------------------
    auto quadCube = std::make_unique<CanvasCube>();
    quadCube.get()->initCanvas();

    skybox.get()->initSkybox();
    std::vector<std::string> faces = {
        "Texture/skybox/right.jpg",  // +X
        "Texture/skybox/left.jpg",   // -X
        "Texture/skybox/top.jpg",    // +Y
        "Texture/skybox/bottom.jpg", // -Y
        "Texture/skybox/front.jpg",  // +Z
        "Texture/skybox/back.jpg"    // -Z
    };
//stbi_set_flip_vertically_on_load(false);
    unsigned int textureCubeMap = skybox.get()->loadCubemap(faces);
//stbi_set_flip_vertically_on_load(true);
    skybox.get()->getShader()->use();
    skybox.get()->getShader()->setInt("skybox", 0);

    shaderMonkey.get()->use();
    shaderMonkey.get()->setInt("skybox", 0);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        // FIRST PASS: render scene to framebuffer
        // ========================================
        glBindFramebuffer(GL_FRAMEBUFFER, quadCube.get()->getFramebuffer());
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE); // active only front faces

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderMonkey.get()->use();
        model = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shaderMonkey.get()->setMat4("view", view);
        shaderMonkey.get()->setMat4("projection", projection);
        shaderMonkey.get()->setMat4("model", model);
        shaderMonkey.get()->setVec3("cameraPos",camera.Position);
        monkey.get()->Draw(*shaderMonkey.get());

        // === RENDER SKYBOX ===
        // Draw skybox as last (important for depth testing optimization)
        glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content
        glDisable(GL_CULL_FACE);

        skybox.get()->getShader()->use();
        skybox.get()->getShader()->setMat4("projection", projection);
        glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skybox.get()->getShader()->setMat4("view", skyboxView);

        skybox.get()->useSkyBox(textureCubeMap);
        glDepthFunc(GL_LESS); // Set depth function back to default

        // === SECOND PASS: Render framebuffer to screen quad ===

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test for screen-space quad
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        quadCube.get()->useCanvas();

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    quadCube.get()->deleteBuffers();

    // glfw: terminate
    // ---------------
    glfwTerminate();
    return 0;
}