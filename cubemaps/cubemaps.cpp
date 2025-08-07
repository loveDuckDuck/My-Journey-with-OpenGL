#include "../util/Umbrella.h"

// Improved cubemap loading with better error handling
unsigned int loadCubemap(const std::vector<std::string> &faces);
unsigned int loadCubemap(const std::vector<std::string> &faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(FileSystem::getPath(faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// Skybox vertex data - organized for better readability
const float skyboxVertices[] = {
    // positions
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f

};

// Lighting configuration structure
struct LightingConfig
{
    glm::vec3 lightConstant = glm::vec3(1.0f);
    float outerCutOff = glm::cos(glm::radians(34.0f));
    float cutOff = glm::cos(glm::radians(25.0f));
    float constantDistance = 1.0f;
    float linearDistance = 0.09f;
    float quadraticDistance = 0.032f;
};

// Render configuration
struct RenderConfig
{
    bool enableFaceCulling = true;
    bool enableDepthTest = true;
    bool enableWireframe = false;
    glm::vec3 clearColor = glm::vec3(0.1f, 0.1f, 0.1f);
};

void setupLighting(Shader &shader, const LightingConfig &config, const Camera &camera, const Material &material)
{
    shader.use();

    // Spotlight configuration
    shader.setVec3("spotLight.position", camera.Position);
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", config.lightConstant * 0.2f); // Dimmer ambient
    shader.setVec3("spotLight.diffuse", config.lightConstant);
    shader.setVec3("spotLight.specular", config.lightConstant);

    shader.setFloat("spotLight.constant", config.constantDistance);
    shader.setFloat("spotLight.linear", config.linearDistance);
    shader.setFloat("spotLight.quadratic", config.quadraticDistance);
    shader.setFloat("spotLight.cutOff", config.cutOff);
    shader.setFloat("spotLight.outerCutOff", config.outerCutOff);

    // Material properties
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setFloat("material.shininess", material.shininess);

    shader.setVec3("viewPos", camera.Position);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window with better title
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Renderer - Monkey & Skybox", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Setup callbacks and context
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Configure OpenGL state
    //stbi_set_flip_vertically_on_load(true);
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_MULTISAMPLE); // Enable multisampling for anti-aliasing

    // Load models
    Model monkey(FileSystem::getPath("monkey.obj"));

    // Create shaders
    Shader shaderMonkey(FileSystem::getPath("Shaders/material_vertex.vs").c_str(),
                        FileSystem::getPath("Shaders/material_fragment.fs").c_str());
    Shader shaderCubeMap(FileSystem::getPath("cubemaps/map.vs").c_str(),
                         FileSystem::getPath("cubemaps/map.fs").c_str());

    // Setup framebuffer canvas
    CanvasCube quadCube;
    quadCube.initCanvas();

    // Material and lighting configuration
    Material material = Materials::SILVER;
    LightingConfig lightConfig;
    RenderConfig renderConfig;

    // Load skybox textures
    std::vector<std::string> faces = {
        "util/skybox/right.jpg",  // +X
        "util/skybox/left.jpg",   // -X
        "util/skybox/top.jpg",    // +Y
        "util/skybox/bottom.jpg", // -Y
        "util/skybox/front.jpg",  // +Z
        "util/skybox/back.jpg"    // -Z
    };

    unsigned int cubemapTexture = loadCubemap(faces);
    if (cubemapTexture == 0)
    {
        std::cerr << "Failed to load skybox textures!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Setup skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    // Configure skybox shader
    shaderCubeMap.use();
    shaderCubeMap.setInt("skybox", 0);

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameMovement = static_cast<float>(camera.GetSpeedCamera() * deltaTime);

        // Process input
        processInput(window);

        // === FIRST PASS: Render scene to framebuffer ===
        glBindFramebuffer(GL_FRAMEBUFFER, quadCube.getFramebuffer());
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE); // active only front faces

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glClearColor(renderConfig.clearColor.r, renderConfig.clearColor.g, renderConfig.clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Setup lighting and render monkey
        setupLighting(shaderMonkey, lightConfig, camera, material);
        shaderMonkey.setMat4("projection", projection);
        shaderMonkey.setMat4("view", view);

        // Render monkey with transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        // Optional: Add rotation for more dynamic scene
        // model = glm::rotate(model, currentFrame * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

        shaderMonkey.setMat4("model", model);
        monkey.Draw(shaderMonkey);

        // === RENDER SKYBOX ===
        // Draw skybox as last (important for depth testing optimization)
        glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content
        glDisable(GL_CULL_FACE);
        shaderCubeMap.use();

        // Remove translation from the view matrix for skybox (so it stays centered on camera)
        glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        shaderCubeMap.setMat4("view", skyboxView);
        shaderCubeMap.setMat4("projection", projection);

        // Render the skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS); // Set depth function back to default

        // === SECOND PASS: Render framebuffer to screen quad ===
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        quadCube.useCanvas();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    quadCube.deleteBuffers();
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteTextures(1, &cubemapTexture);

    glfwTerminate();
    return 0;
}