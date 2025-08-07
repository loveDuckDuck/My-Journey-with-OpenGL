#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>

#include "../util/Random.h"
#include "../util/Texture.h"
#include "../util/Shader.h"
#include "../util/Cube.h"
#include "../util/Camera.h"
#include "../util/Filesystem.h"
#include "../util/Callback.h"

int main()
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
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // here im gone insert all the Shader
    // -----------------------------------

    Shader ligthShader("light_shader.vs", "light_fragment.fs");
    Shader cuberShader("more_ligth_shader.vs", "more_ligth_fragment.fs");

    // random position of cubes
    // --------------------------------------------

    Random random;
    glm::vec3 cubePositions[] = {
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3),
        random.GenerateVec3(-3, 3)};

    // insert all the meshes
    // -----------------------------------
    Cube cube;
    unsigned int VAOCube, VBO;
    glGenVertexArrays(1, &VAOCube); // define the vertex arrey of my obecjt
    glGenBuffers(1, &VBO);          // contains all the data of the object on the GPU

    glBindVertexArray(VAOCube);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cube.getVertexDataSize(), cube.getVertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create the light source in my world so i can use it
    // basiccaly is a simple stupid cube the real magic happend in while(true)

    unsigned int VAOLight;
    glGenVertexArrays(1, &VAOLight);

    glBindVertexArray(VAOLight);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // insert all the maps and load that to my shader
    // -----------------------------------------------

    unsigned int diffuseMap = Texture::loadTexture(FileSystem::getPath("container2.png").c_str());
    unsigned int specularMap = Texture::loadTexture(FileSystem::getPath("container2_specular.png").c_str());

    cuberShader.use();
    cuberShader.setInt("material.diffuse", 0);
    cuberShader.setInt("material.specular", 1);

    // color of the object
    // and color of the light
    // --------------------------
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // define the light position in the world
    // ---------------------------------------
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    // here i am gone insert all the static value that i dont need toe ridefine in the
    // loop
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
    std::string pointsLightString = "pointLights";

    glm::vec3 pointLightsAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 pointLightsDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 pointLightsSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    float pointLightsConstant = 1.0f;
    float pointLightsLinear = 0.09f;
    float pointLightsQuadric = 0.032f;

    // spotlight costant
    // ------------------

    glm::vec3 spotLightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        // define the deltatime
        // -------------------------------
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameMovement = static_cast<float>(camera.GetSpeedCamera() * deltaTime);

        // process all the inputs from the I/O
        // -----------------------------------
        processInput(window);

        // background color and macro for the delth buffer
        // -----------------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // define all the cube shader variable and set that

        cuberShader.use();
        cuberShader.setVec3("viewPos", camera.Position);
        cuberShader.setFloat("material.shininess", 32.0f);

        // directional light
        // ------------------
        cuberShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        cuberShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        cuberShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        cuberShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // insert all the element
        // shaders, matrix, light etc...
        // ----------------------------------

        // define the pointerlight in the scene
        // --------------------------------------

        for (unsigned int i = 0; i < 4; ++i)
        {
            auto templatepointerString = pointsLightString + "[" + std::to_string(i) + "].";

            // position
            // ---------------
            cuberShader.setVec3(templatepointerString + "position", pointLightPositions[i]);
            // constance, distance and quadratic
            // ----------------------------------
            cuberShader.setFloat(templatepointerString + "constant", pointLightsConstant);
            cuberShader.setFloat(templatepointerString + "linear", pointLightsLinear);
            cuberShader.setFloat(templatepointerString + "quadratic", pointLightsQuadric);

            // define the ambient, specular and the diffuse
            cuberShader.setVec3(templatepointerString + "ambient", pointLightsAmbient);
            cuberShader.setVec3(templatepointerString + "diffuse", pointLightsDiffuse);
            cuberShader.setVec3(templatepointerString + "specular", pointLightsSpecular);
        }
        // spotLight
        cuberShader.setVec3("spotLight.position", camera.Position);
        cuberShader.setVec3("spotLight.direction", camera.Front);
        cuberShader.setVec3("spotLight.ambient", spotLightAmbient);
        cuberShader.setVec3("spotLight.diffuse", spotLightDiffuse);
        cuberShader.setVec3("spotLight.specular", spotLightDiffuse);
        cuberShader.setFloat("spotLight.constant", 1.0f);
        cuberShader.setFloat("spotLight.linear", 0.09f);
        cuberShader.setFloat("spotLight.quadratic", 0.032f);
        cuberShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        cuberShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection =  glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        cuberShader.setMat4("projection", projection);
        cuberShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        cuberShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glBindVertexArray(VAOCube);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
            cuberShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // also draw the lamp object(s)
        ligthShader.use();
        ligthShader.setVec3("aColor", glm::vec3(lightColor));

        ligthShader.setMat4("projection", projection);
        ligthShader.setMat4("view", view);
        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(VAOLight);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            ligthShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
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