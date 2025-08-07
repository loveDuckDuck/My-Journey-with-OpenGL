#ifndef QUADCUBE_H
#define QUADCUBE_H
#include "Shader.h"
#include <iostream>
#include <memory>
#include "UtilDimension.h"
class CanvasCube
{
private:
    std::unique_ptr<Shader> shader;
    const float quadVertices[30] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int quadVAO, quadVBO;
    /*The Custom Canvas (Framebuffer)
    First, the system creates a new drawing surface -
    think of it as setting up a blank canvas that has the exact
    same dimensions as your screen, but exists only in memory.
    */
    unsigned int framebuffer;
    /*
    The Color Layer (Color Attachment)
    Next, it creates a special texture that will
    capture all the colors of whatever gets drawn.
    This is like preparing a high-quality photographic paper that
    can record every pixel of color information.
    The texture is configured to be smooth when viewed up close or from far away.
    */
    unsigned int textureColorbuffer;
    /*The Depth Layer (Renderbuffer)
    Finally, it adds a depth tracking system.
    This is crucial for 3D rendering because it keeps
    track of which objects are in front of others. Imagine
    it as an invisible layer that remembers how far away
    each pixel is from the viewer,
    ensuring that a nearby object will properly hide objects behind it.*/
    unsigned int rbo;

public:
    CanvasCube(/* args */);
    ~CanvasCube();
    void initCanvas();
    void useCanvas();

    unsigned int getFramebuffer();
};
// Constructor
CanvasCube::CanvasCube()
{
    shader = std::make_unique<Shader>("screen.vs", "screen.fs");

    // Initialize OpenGL handles to 0
    quadVAO = quadVBO = 0;
    framebuffer = textureColorbuffer = rbo = 0;
}

// Destructor - Clean up OpenGL resources
CanvasCube::~CanvasCube()
{
    if (quadVAO != 0)
        glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO != 0)
        glDeleteBuffers(1, &quadVBO);
    if (framebuffer != 0)
        glDeleteFramebuffers(1, &framebuffer);
    if (textureColorbuffer != 0)
        glDeleteTextures(1, &textureColorbuffer);
    if (rbo != 0)
        glDeleteRenderbuffers(1, &rbo);

    // shader unique_ptr automatically cleans itself up
}

void CanvasCube::initCanvas()
{
    // Use class members directly - NO local variable declarations
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // Framebuffer configuration
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Create renderbuffer for depth and stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Configure shader
    shader->use(); // Can use -> instead of .get()->
    shader->setInt("screenTexture", 0);
}

void CanvasCube::useCanvas()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test for screen-space quad
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

unsigned int CanvasCube::getFramebuffer()
{
    return framebuffer;
}

#endif