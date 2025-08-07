#ifndef QUADCUBE_H
#define QUADCUBE_H
#include "Shader.h"
#include <iostream>
#include <memory>
#include "UtilDimension.h"
#include "Filesystem.h"

class CanvasCube
{
private:
    std::unique_ptr<Shader> shader;
    
    // Fixed quad vertices - the original had incorrect array size and positioning
    const float quadVertices[24] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,  // top left
        -1.0f, -1.0f,  0.0f, 0.0f,  // bottom left
         1.0f, -1.0f,  1.0f, 0.0f,  // bottom right

        -1.0f,  1.0f,  0.0f, 1.0f,  // top left
         1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
         1.0f,  1.0f,  1.0f, 1.0f   // top right
    };
    
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
    CanvasCube();
    ~CanvasCube();
    void initCanvas();
    void useCanvas();
    unsigned int getFramebuffer();
    void deleteBuffers();
};

// Constructor
CanvasCube::CanvasCube()
{
    shader = std::make_unique<Shader>(
        FileSystem::getPath("Shaders/screen.vs").c_str(), 
        FileSystem::getPath("Shaders/screen.fs").c_str()
    );

    // Initialize OpenGL handles to 0
    quadVAO = quadVBO = 0;
    framebuffer = textureColorbuffer = rbo = 0;
}

// Destructor
CanvasCube::~CanvasCube()
{
    // Destructor should call cleanup, but we'll keep explicit deleteBuffers() call
}

void CanvasCube::deleteBuffers()
{
    if (quadVAO != 0) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO != 0) {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    if (textureColorbuffer != 0) {
        glDeleteTextures(1, &textureColorbuffer);
        textureColorbuffer = 0;
    }
    if (rbo != 0) {
        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }
}

void CanvasCube::initCanvas()
{
    // === SETUP SCREEN QUAD ===
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // Texture coordinate attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0); // Unbind VAO

    // === SETUP FRAMEBUFFER ===
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Create renderbuffer for depth and stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check framebuffer completeness
    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! Status: " << framebufferStatus << std::endl;
        
        // Print specific error information
        switch(framebufferStatus) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
                break;
            default:
                std::cout << "Unknown framebuffer error" << std::endl;
        }
    } else {
        std::cout << "Framebuffer setup successful!" << std::endl;
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Configure shader
    shader->use();
    shader->setInt("screenTexture", 0);
    
    // Debug: Check if shader is working
    std::cout << "CanvasCube initialized successfully" << std::endl;
}

void CanvasCube::useCanvas()
{
    // Ensure we're rendering to the default framebuffer (screen)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    shader->use();
    glBindVertexArray(quadVAO);
    
    // Bind the framebuffer texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    
    // Draw the quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int CanvasCube::getFramebuffer()
{
    return framebuffer;
}

#endif